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

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/CSDAG/CSDAGGraph.h"

void createTestCom(CSDAGGraph* graph, Architecture* archi){
	// Generate actor
	CSDAGVertex* vGenerate = graph->addVertex("Generate");
	vGenerate->addParam("tabSize");
	vGenerate->setFunctionIndex(1);

	vGenerate->addTiming(0,"tabSize");
	vGenerate->addAllConstraints(1, archi->getNbSlaves());

	// Transform1 actor
	CSDAGVertex* vTransform1 = graph->addVertex("Transform1");
	vTransform1->addParam("consoSize1");
	vTransform1->setFunctionIndex(2);

	vTransform1->addTiming(0,"consoSize1");
	vTransform1->addAllConstraints(1, archi->getNbSlaves());

	// Transform2 actor
	CSDAGVertex* vTransform2 = graph->addVertex("Transform2");
	vTransform2->addParam("consoSize2");
	vTransform2->setFunctionIndex(3);

	vTransform2->addTiming(0,"consoSize2");
	vTransform2->addAllConstraints(1, archi->getNbSlaves());

	// Verify actor
	CSDAGVertex* vVerify = graph->addVertex("Verify");
	vVerify->addParam("tabSize");
	vVerify->setFunctionIndex(4);

	vVerify->addTiming(0,"tabSize");
	vVerify->addAllConstraints(1, archi->getNbSlaves());

	// Edges
	graph->addEdge(vGenerate,"{tabSize}",vTransform1,"{consoSize1}");
	graph->addEdge(vTransform1,"{consoSize1}",vTransform2,"{consoSize2}");
	graph->addEdge(vTransform2,"{consoSize2}",vVerify,"{tabSize}");
}

void createZynqArchi(Architecture* archi, int nbSlaves){
	static char tempStr[11];
	// Architecture Zynq
	archi->addSlave(0, "ARM", 0.410, 331, 0.4331, 338);
	for(int i=1; i<nbSlaves; i++){
		sprintf(tempStr,"uBlaze%02d",i);
		archi->addSlave(1, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

void createSobel(CSDAGGraph* graph, Architecture* archi){
	/* READYUV Actor */
	CSDAGVertex* vCapture = graph->addVertex("ReadYUV");
	vCapture->addParam("Width");
	vCapture->addParam("Height");
	vCapture->addParam("Color");
	vCapture->setFunctionIndex(1);

	vCapture->addTiming(0,"139305-Color*10191");
	vCapture->addAllConstraints(1, 1);

	/* SPLIT Actor */
	CSDAGVertex* vSplit = graph->addVertex("Split");
	vSplit->addParam("Width");
	vSplit->addParam("Height");
	vSplit->addParam("nbSlices");
	vSplit->setFunctionIndex(2);

	vSplit->addTiming(0,"190392-nbSlices*2551");
	vSplit->addAllConstraints(1, 1);

	/* SOBEL Actor */
	CSDAGVertex* vSobel = graph->addVertex("Sobel");
	vSobel->addParam("Width");
	vSobel->addParam("Height");
	vSobel->addParam("nbSlices");
	vSobel->setFunctionIndex(3);

	vSobel->addTiming(0,"417/100*Width*Height/nbSlices");
	vSobel->addTiming(1,"4321/100*Width*Height/nbSlices");
	vSobel->addAllConstraints(1, archi->getNbSlaves());

	/* MERGE Actor */
	CSDAGVertex* vMerge = graph->addVertex("Merge");
	vMerge->addParam("Width");
	vMerge->addParam("Height");
	vMerge->addParam("nbSlices");
	vMerge->setFunctionIndex(4);

	vMerge->addTiming(0,"92350-nbSlices*355");
	vMerge->addAllConstraints(1, 1);

	/* DISPLAYYUV Actor */
	CSDAGVertex* vDisplay = graph->addVertex("DisplayYUV");
	vDisplay->addParam("Width");
	vDisplay->addParam("Height");
	vDisplay->addParam("Color");
	vDisplay->setFunctionIndex(5);

	vDisplay->addTiming(0,"1097840-Color*36523");
	vDisplay->addAllConstraints(1,1);

	// Edges
	graph->addEdge(vCapture,"{Width*Height}",vSplit,"{Width*Height}");
	graph->addEdge(vSplit  ,"{Width*(Height/nbSlices+2)*nbSlices}",vSobel,"{Width*(Height/nbSlices+2)}");
	graph->addEdge(vSobel  ,"{Width*(Height/nbSlices+2)}",vMerge,"{Width*(Height/nbSlices+2)*nbSlices}");
	graph->addEdge(vMerge  ,"{Width*Height}",vDisplay,"{Width*Height}");
	graph->addEdge(vCapture,"{Color*Width*Height/4}",vDisplay,"{Color*Width*Height/4}");
	graph->addEdge(vCapture,"{Color*Width*Height/4}",vDisplay,"{Color*Width*Height/4}");
}
