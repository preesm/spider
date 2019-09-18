/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Daniel Madroñal <daniel.madronal@upm.es> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
 * rlazcano <raquel.lazcano@upm.es> (2019)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include <cinttypes>
#include "energyAwareness.h"
#include <algorithm>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <limits>
#include <math.h>

#include "spider.h"

#ifdef __k1__
#define CHIP_FREQ ((float)(__bsp_frequency)/(1000*1000))
#endif
#ifndef CHIP_FREQ
#define CHIP_FREQ (1)
#endif

// energy awareness info
static Time startingExecutionTime_;
static Time endingExecutionTime_;

static std::map<std::uint32_t, std::vector<std::uint32_t>> peIdPerPeType_; 
static std::map<std::uint32_t, std::uint32_t> pesBeingDisabled_; 

static std::map<const char*, Param> dynamicParameters_;
static std::vector<std::map<std::uint32_t, std::uint32_t>> configsAlreadyUsed_; 
static std::map<std::uint32_t, std::uint32_t> pesBestConfig_; 
static double bestEnergy_;
static double bestObjective_;
static bool energyAlreadyOptimized_;
static bool solutionShown_;
static bool aboveBelowObjective_; // 1 current config is above objective -- 0 current config is below objective
static std::uint32_t timesObjectiveMissed_; // Fail safe when not reaching the objective
static bool forcedUpdate_; // True when the performance of the best config does not reach the objective 10 times in a row
static bool coarseFineGrainSearching_; // True when we are performing coarse-grain searching; false if it is fine grain
static std::map<std::uint32_t, std::uint32_t> upperPENumber_; 
static std::map<std::uint32_t, std::uint32_t> lowerPENumber_; 

static std::map<std::map<const char*, Param>, std::vector<std::map<std::uint32_t, std::uint32_t>>> configsAlreadyUsedBackup_; 
static std::map<std::map<const char*, Param>, std::map<std::uint32_t, std::uint32_t>> pesBestConfigBackup_; 
static std::map<std::map<const char*, Param>, std::map<std::uint32_t, std::uint32_t>> pesBeingDisabledBackup_; 
static std::map<std::map<const char*, Param>, double> bestEnergyBackup_; 
static std::map<std::map<const char*, Param>, double> bestObjectiveBackup_; 
static std::map<std::map<const char*, Param>, bool> energyAlreadyOptimizedBackup_; 
static std::map<std::map<const char*, Param>, bool> aboveBelowObjectiveBackup_; 
static std::map<std::map<const char*, Param>, bool> coarseFineGrainSearchingBackup_; 
static std::map<std::map<const char*, Param>, std::map<std::uint32_t, std::uint32_t>> upperPENumberBackup_; 
static std::map<std::map<const char*, Param>, std::map<std::uint32_t, std::uint32_t>> lowerPENumberBackup_; 

void EnergyAwareness::setStartingTime() {
    startingExecutionTime_ = Platform::get()->getTime();
}

void EnergyAwareness::setEndTime() {
    endingExecutionTime_ = Platform::get()->getTime();
}

unsigned long EnergyAwareness::getExecutionTime() {
    return (endingExecutionTime_ - startingExecutionTime_) / CHIP_FREQ;
}

double EnergyAwareness::computeEnergy(SRDAGGraph *srdag, Archi *archi, double fpsEstimation) {
    double energyIter = 0.0;
    std::vector<std::uint32_t> pesUsed;
    for (int i = 0; i < srdag->getNVertex(); i++) {
        SRDAGVertex *vertex = srdag->getVertex(i);
        if (vertex->getType() == SRDAG_NORMAL) {
            int peUsed = vertex->getScheduleJob()->getMappedPE();
            int peType = archi->getPEFromSpiderID(peUsed)->getHardwareType();
            auto piVertex = vertex->getReference();
            double energy = piVertex->getEnergyOnPEType(peType);
            energyIter = energyIter + energy;      
        }
    }
    // we asume that energy for each actor is given in uJ
    double energyApp = (energyIter / (1000.0 * 1000.0))  * fpsEstimation;
    // as power is provided in W and we want energy per second, values are equivalent
    double energyPlatform = archi->getBasePower();
    for (unsigned int i = 0; i < archi->getNPE(); i++) {
        auto pe = archi->getPEFromSpiderID(i);
        if (pe->isEnabled()) {
            energyPlatform = energyPlatform + pe->getPower();            
        }
    }
    return energyApp + energyPlatform;    
}

double EnergyAwareness::computeFps() {
    unsigned long execTime = EnergyAwareness::getExecutionTime();
    // execution time is given in ns
    return (1000.0 * 1000.0 * 1000.0) / (double) execTime;
}

void EnergyAwareness::setUp(Archi *archi) {
    auto spiderId = archi->getSpiderGRTID();
    for (std::uint32_t i = 0; i < archi->getNPE(); i++) {
        auto pe = archi->getPEFromSpiderID(i);
        if (i != spiderId) {
            auto peType = pe->getHardwareType();
            auto it = peIdPerPeType_.find(peType);
            if (it != peIdPerPeType_.end()) {
                it->second.push_back(i);
            } else {
                std::vector<std::uint32_t> peIdSet;
                peIdSet.push_back(i);
                peIdPerPeType_.insert(std::make_pair(peType, peIdSet));
            }          
        }
    }
    for (auto it = peIdPerPeType_.begin(); it != peIdPerPeType_.end(); it++) {
        pesBeingDisabled_.insert(std::make_pair(it->first, 0));
        pesBestConfig_.insert(std::make_pair(it->first, 0));
        upperPENumber_.insert(std::make_pair(it->first, (std::uint32_t)it->second.size()));
        lowerPENumber_.insert(std::make_pair(it->first, 0));
    }
    energyAlreadyOptimized_ = false;
    solutionShown_ = false;
    bestEnergy_ = std::numeric_limits<double>::max();
    bestObjective_ = 0.0;
    timesObjectiveMissed_ = 0;
    forcedUpdate_ = false;
    coarseFineGrainSearching_ = true;
}

void EnergyAwareness::checkExecutionPerformance(double fpsEstimation, double energyConsumed) {
    aboveBelowObjective_ = false;
    if (fpsEstimation >= Spider::getPerformanceObjective()) {
        aboveBelowObjective_ = true;
        if (bestEnergy_ > energyConsumed || forcedUpdate_) {
            bestEnergy_ = energyConsumed;
            bestObjective_ = fpsEstimation;
            for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
                pesBestConfig_[it->first] = it->second;
            }
        } 
    }else if (fpsEstimation > bestObjective_ && bestEnergy_ == std::numeric_limits<double>::max()) {
        bestObjective_ = fpsEstimation;
        for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
            pesBestConfig_[it->first] = it->second;
        }
    }
    forcedUpdate_ = false;
}

bool EnergyAwareness::generateNextConfiguration() {
    if(coarseFineGrainSearching_){
        return generateNextCoarseGrainConfiguration();
    }else{
        return generateNextFineGrainConfiguration();
    }
}

bool EnergyAwareness::generateNextCoarseGrainConfiguration(){
    for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
        if(aboveBelowObjective_){ // As we are modifying the number of disabled PEs, we have to increase the disable PEs when we are above the number
            lowerPENumber_[it->first] = pesBeingDisabled_[it->first];
            pesBeingDisabled_[it->first] = ceil((pesBeingDisabled_[it->first] + upperPENumber_[it->first]) / 2.0);
        } else {
            upperPENumber_[it->first] = pesBeingDisabled_[it->first];
            pesBeingDisabled_[it->first] = floor((pesBeingDisabled_[it->first] + lowerPENumber_[it->first]) / 2.0); 
        }
    }
    auto it = std::find(configsAlreadyUsed_.begin(), configsAlreadyUsed_.end(), pesBeingDisabled_);
    if (it == configsAlreadyUsed_.end()) {
        return true;
    } else {
        coarseFineGrainSearching_ = false;
        return false;        
    }
}

bool EnergyAwareness::generateNextFineGrainConfiguration(){
    bool alreadyUsingTheMaximum = true;
    for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
        if(aboveBelowObjective_){ // As we are modifying the number of disabled PEs, we have to increase the disable PEs when we are above the number
            if(pesBeingDisabled_[it->first] == peIdPerPeType_[it->first].size()){
                pesBeingDisabled_[it->first] = 0;
            }else{
                pesBeingDisabled_[it->first] = pesBeingDisabled_[it->first] + 1; 
                break;
            }      
        } else {
            if(pesBeingDisabled_[it->first] == 0){
                pesBeingDisabled_[it->first] = peIdPerPeType_[it->first].size();
            }else{
                pesBeingDisabled_[it->first] = pesBeingDisabled_[it->first] - 1;  
                alreadyUsingTheMaximum = false;
                break;
            }      
        }
    }
    auto it = std::find(configsAlreadyUsed_.begin(), configsAlreadyUsed_.end(), pesBeingDisabled_);
    if (it == configsAlreadyUsed_.end()) {
        return true;
    } else if(!aboveBelowObjective_ && bestEnergy_ != std::numeric_limits<double>::max() && !alreadyUsingTheMaximum){
        timesObjectiveMissed_++;
        if(timesObjectiveMissed_ == 10){
            timesObjectiveMissed_ = 0;
            forcedUpdate_ = true;
            printf("Increasing number of enabled PEs because the objective has been missed 10 times\n");
            return true;
        }
    }
    return false;
}

void EnergyAwareness::applyConfig(Archi *archi){
    auto it = std::find(configsAlreadyUsed_.begin(), configsAlreadyUsed_.end(), pesBeingDisabled_);
    if (it == configsAlreadyUsed_.end()) {
        configsAlreadyUsed_.push_back(pesBeingDisabled_);
    }
    std::map<std::uint32_t, std::uint32_t> pesAlreadyDisbled;
    for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
        pesAlreadyDisbled.insert(std::make_pair(it->first, 0));
    }
    for(auto it = peIdPerPeType_.begin(); it != peIdPerPeType_.end(); it++){
        for(auto itInner = it->second.begin(); itInner != it->second.end(); itInner++){
            archi->activatePE(archi->getPEFromSpiderID(*itInner));
        }
    }
    for(auto it = peIdPerPeType_.begin(); it != peIdPerPeType_.end(); it++){
        for(auto itInner = it->second.begin(); itInner != it->second.end(); itInner++){
            if(pesAlreadyDisbled[it->first] == pesBeingDisabled_[it->first]){
                break;
            }
            archi->deactivatePE(archi->getPEFromSpiderID(*itInner));
            pesAlreadyDisbled[it->first] = pesAlreadyDisbled[it->first] + 1;
        }
    }
}

void EnergyAwareness::analyzeExecution(SRDAGGraph *srdag, Archi *archi){
    double fpsEstimation = computeFps();
    double energyConsumed = computeEnergy(srdag, archi, fpsEstimation);

    checkExecutionPerformance(fpsEstimation, energyConsumed);
}

void EnergyAwareness::prepareNextExecution(){
    if(!generateNextConfiguration()){
        energyAlreadyOptimized_ = true;
        pesBeingDisabled_ = pesBestConfig_;
        for (auto it = pesBestConfig_.begin(); it != pesBestConfig_.end(); it++) {
            pesBeingDisabled_[it->first] = it->second;
        }
        if(!solutionShown_){
            solutionShown_ = true;
            std::uint32_t totalCoresUsed = 0;
            for (auto it = pesBestConfig_.begin(); it != pesBestConfig_.end(); it++) {
                totalCoresUsed = totalCoresUsed + peIdPerPeType_[it->first].size() - it->second;
            }
            if(bestEnergy_ == std::numeric_limits<double>::max()){
                printf("Best FPS %f. Objective not reachable --> ", bestObjective_);    
                printf("Closer one reached using %d cores", (totalCoresUsed + 1)); // We include here the GRT
            }else{
                printf("Best FPS %f and best energy consumed = %f ---> ", bestObjective_, bestEnergy_);   
                printf("Reached using %d cores", (totalCoresUsed + 1)); // We include here the GRT             
            }
            printf("\n");
            printf("Using config (PEType: disabled/total): ");
            for (auto it = pesBestConfig_.begin(); it != pesBestConfig_.end(); it++) {
                printf("%d: %d/%lu --- ", it->first, it->second, peIdPerPeType_[it->first].size());
            }
            printf("\n");
        }
    }
}

void EnergyAwareness::recoverConfigOrDefault() {
    timesObjectiveMissed_ = 0;
    // All the energy-awareness related variables are stored at the same time
    // So we only check one and we consider that everything is properly done
    auto itCheckerEnergyAlreadyOptimized = energyAlreadyOptimizedBackup_.find(dynamicParameters_);
    if(itCheckerEnergyAlreadyOptimized != energyAlreadyOptimizedBackup_.end()){
        energyAlreadyOptimized_ = energyAlreadyOptimizedBackup_[dynamicParameters_];
        coarseFineGrainSearching_ = coarseFineGrainSearchingBackup_[dynamicParameters_];
        aboveBelowObjective_ = aboveBelowObjectiveBackup_[dynamicParameters_];
        pesBestConfig_ = pesBestConfigBackup_[dynamicParameters_];
        pesBeingDisabled_ = pesBeingDisabledBackup_[dynamicParameters_]; 
        bestEnergy_ = bestEnergyBackup_[dynamicParameters_];
        bestObjective_ = bestObjectiveBackup_[dynamicParameters_];
        configsAlreadyUsed_ = configsAlreadyUsedBackup_[dynamicParameters_];
        upperPENumber_ = upperPENumberBackup_[dynamicParameters_];
        lowerPENumber_ = lowerPENumberBackup_[dynamicParameters_];
    }else{
        energyAlreadyOptimized_ = false;        
        for (auto it = pesBeingDisabled_.begin(); it != pesBeingDisabled_.end(); it++) {
            pesBestConfig_[it->first] = it->second;
        }
        bestEnergy_ = std::numeric_limits<double>::max();
        bestObjective_ = 0.0;
        configsAlreadyUsed_.clear();
        coarseFineGrainSearching_ = true;
    }
    solutionShown_ = false;
}

void EnergyAwareness::setNewDynamicParams(std::map<const char*, Param> dynamicParamsMap) {
    //Store current best ones
    configsAlreadyUsedBackup_[dynamicParameters_] = configsAlreadyUsed_; 
    pesBestConfigBackup_[dynamicParameters_] = pesBestConfig_; 
    pesBeingDisabledBackup_[dynamicParameters_] = pesBeingDisabled_; 
    bestEnergyBackup_[dynamicParameters_] = bestEnergy_; 
    bestObjectiveBackup_[dynamicParameters_] = bestObjective_; 
    coarseFineGrainSearchingBackup_[dynamicParameters_] = coarseFineGrainSearching_;
    energyAlreadyOptimizedBackup_[dynamicParameters_] = energyAlreadyOptimized_;
    aboveBelowObjectiveBackup_[dynamicParameters_] = aboveBelowObjective_;
    upperPENumberBackup_[dynamicParameters_] = upperPENumber_;
    lowerPENumberBackup_[dynamicParameters_] = lowerPENumber_;

    //Check the need to update the config
    bool dirtyEnergyConfig = false;
    for (auto it = dynamicParamsMap.begin(); it != dynamicParamsMap.end(); it++) {
        auto itChecker = dynamicParameters_.find(it->first);
        if(itChecker == dynamicParameters_.end()){
            dynamicParameters_.insert(std::make_pair(it->first, it->second));
            dirtyEnergyConfig = true;
        } else {
            if(dynamicParameters_[it->first] != it->second){
                dirtyEnergyConfig = true;
                dynamicParameters_[it->first] = it->second;
            }
        }        
    }
    //Update config if needed
    if(dirtyEnergyConfig){
        recoverConfigOrDefault();
    }
}

bool EnergyAwareness::getEnergyAlreadyOptimized(){
    return energyAlreadyOptimized_;
}
