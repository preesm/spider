/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <spider.h>
#include <grt_definitions.h>

void resetGraph();

PiSDFGraph* initPisdf_stereo(
		PiSDFGraph* _graphs,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap);

int main(){
#ifdef DSP
	int nbSlaves = 8;
#else
	int nbSlaves = 1;
#endif

	static Architecture 		arch;
	static ExecutionStat 		execStat;
	static PiSDFGraph 			pisdfGraphs[MAX_NB_PiSDF_GRAPHS];
	static PiSDFGraph 			*topPisdf;

	static int time[20];

	printf("Starting with %d slaves max\n", nbSlaves);

	/* Create the architecture */
	arch.reset();
	arch.addSlave(0, "Master", 0.9267, 435, 0.9252, 430);
	char tempStr[MAX_SLAVE_NAME_SIZE];

	for(int i=1; i<nbSlaves; i++){
		UINT32 len = snprintf(tempStr, MAX_SLAVE_NAME_SIZE, "PE%02d", i);
		if(len > MAX_SLAVE_NAME_SIZE){
			exitWithCode(1073);
		}
		arch.addSlave(0, tempStr, 0.9267, 435, 0.9252, 430);
	}
	arch.setNbActiveSlaves(nbSlaves);

//	setFctPtr(0, config);
//	setFctPtr(1, mFilter);
//	setFctPtr(2, src);
//	setFctPtr(3, snk);
//	setFctPtr(4, setM);
//	setFctPtr(5, initSwitch);
//	setFctPtr(7, FIR);

	/*
	 * These objects should be obtained from the PREESM generator :
	 * Architecture, Scheduling policy.
	 */

	SPIDER_init(&arch);

	for(int iter=9; iter<=9; iter++){
		printf("N=%d\n", iter);

		resetGraph();
#ifdef DSP
		topPisdf = initPisdf_mpSched(pisdfGraphs, 20, 4000, iter, 0);
#else
		topPisdf = initPisdf_stereo(pisdfGraphs,
				375, 450, 1, 0, 1, 4, 12, 1, 1);
#endif
		SPIDER_reset();

		SPIDER_launch(&arch, topPisdf);

		SPIDER_report(&arch, topPisdf, &execStat, iter);

 		printf("GraphTime:   %d\n", execStat.graphTransfoTime);
 		printf("MappingTime: %d\n", execStat.mappingTime);
 		printf("TaskOrdTime: %d\n", execStat.taskOrderingTime);

 		printf("\nEndTime: %d\n", execStat.globalEndTime);

 		time[iter-1] = execStat.globalEndTime;
	}

	char file[100];
	printf("time\n");
	sprintf(file,"spider_cache_nvar.csv");
	FILE *f = fopen(file,"w+");
	fprintf(f, "iter, latency\n");
	for(int iter=1; iter<=18; iter++){
		fprintf(f,"%d,%d\n",iter, time[iter-1]);
		printf("%d: %d\n",iter, time[iter-1]);
	}
	fclose(f);

	printf("finished\n");
}
