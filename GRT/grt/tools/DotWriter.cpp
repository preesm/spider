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

/**
 * Writes a dot file from a graph of a given type
 */
#include "DotWriter.h"
#include <string.h>
#include <platform_file.h>

/**
 Writes a SRDAGGraph in a file

 @param graph: written graph
 @param path: output file path
 @param displayName: 1 if the graph should display the vertices names
*/
void DotWriter::write(SRDAGGraph* graph, const char* path, BOOL displayNames, BOOL displayRates){
	//char directory[_MAX_PATH];
	//getcwd(directory, sizeof(directory));
	char name[MAX_VERTEX_NAME_SIZE];
	char color[6];

	platform_fopen (path);
	// Writing header
	platform_fprintf ("digraph srDag {\n");
	platform_fprintf ("node [color=Black];\n");
	platform_fprintf ("rankdir=LR;\n");
	platform_fprintf ("edge [color=Red];\n");
//		platform_fprintf ("rankdir=LR;\n");

	SRDAGVertex* vertex;
	vertexSetIterator iterV = graph->getVertexIterator();
	while((vertex = iterV.next()) != NULL){
		if(vertex->getState() != SrVxStDeleted){
			vertex->getName(name, MAX_VERTEX_NAME_SIZE);

			switch (vertex->getState()) {
				case SrVxStExecutable:
					strcpy(color, "blue");
					break;
				case SrVxStExecuted:
					strcpy(color, "gray");
					break;
				case SrVxStNoExecuted:
					if(vertex->isHierarchical())
						strcpy(color, "red");
					else
						strcpy(color, "black");
					break;
				case SrVxStDeleted:
					break;
			}

			if(displayNames){
				platform_fprintf ("\t%d [label=\"%d\\n%s\" color=\"%s\"];\n",vertex->getId(), vertex->getId(), name, color);
			}
			else{
				platform_fprintf ("\t%d [label=\"%d\" color=\"%s\"];\n",i,i, color);
			}
		}
	}

	SRDAGEdge* edge;
	edgeSetIterator iterE = graph->getEdgeIterator();
	while((edge = iterE.next()) != NULL){
		SRDAGVertex* vxSrc = edge->getSource();
		SRDAGVertex* vxSnk = edge->getSink();
		if((vxSrc->getState() != SrVxStDeleted) && (vxSnk->getState() != SrVxStDeleted)){
			if(displayRates)
				platform_fprintf ("\t%d->%d [label=\"%d\",taillabel=\"%d\",headlabel=\"%d\"];\n",
						edge->getSource()->getId(),
						edge->getSink()->getId(),
						edge->getTokenRate(),
						edge->getSourcePortIx(),
						edge->getSinkPortIx()
						);
			else
				platform_fprintf ("\t%d->%d [label=\"%d: %#x\"];\n", edge->getSource()->getId(), edge->getSink()->getId(), edge->getFifoId(), edge->getFifoAddress());
		}
	}
	platform_fprintf ("}\n");

	platform_fclose ();
}


static void draw_vertex(PiSDFAbstractVertex* vertex, char displayNames, bool drawParameters = true){
	if(displayNames){
		platform_fprintf ("\t%s [label=\"%s\"];\n", vertex->getName(), vertex->getName());
	}
	else{
		platform_fprintf ("\t%s [label=\"\"];\n", vertex->getName());
	}

	if(drawParameters)
		// Drawing lines : parameter -> vertex.
		for (int i = 0; i < vertex->getNbParameters(); i++) {
			PiSDFParameter* param = vertex->getParameter(i);
			platform_fprintf("\t%s->%s [style=dotted];\n", param->getName(), vertex->getName());
		}
}



/**
 Writes a PiSDFGraph in a file

 @param graph: written graph
 @param path: output file path
*/
void DotWriter::write(PiSDFGraph* graph, const char* path, char displayNames){
	//char directory[_MAX_PATH];
	//getcwd(directory, sizeof(directory));

	platform_fopen (path);
	// Writing header
	platform_fprintf ("digraph csdag {\n");
	platform_fprintf ("node [color=\"#433D63\"];\n");
	platform_fprintf ("edge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	platform_fprintf ("rankdir=LR;\n");

	// Drawing parameters.
	for (UINT64 i=0 ; i<graph->getNb_parameters(); i++)
	{
 		PiSDFParameter* param = graph->getParameter(i);
		if(displayNames){
			platform_fprintf ("\t%s [label=\"%s\" shape=house];\n", param->getName(), param->getName());
		}
		else{
			platform_fprintf ("\t%s [label=\"\" shape=house];\n", param->getName());
		}
	}

	// Drawing configuration vertices.
	for (UINT32 i=0 ; i < graph->getNb_config_vertices(); i++)
	{
		PiSDFConfigVertex* vertex = graph->getConfig_vertex(i);
//			draw_vertex(vertex, displayNames);
		if(displayNames){
			platform_fprintf ("\t%s [label=\"%s\"];\n",vertex->getName(),vertex->getName());
		}
		else{
			platform_fprintf ("\t%s [label=\"\"];\n",vertex->getName());
		}

		// Drawing lines : vertex -> parameters.
		for (UINT32 j = 0; j < vertex->getNbRelatedParams(); j++) {
			platform_fprintf("\t%s->%s [style=dotted];\n", vertex->getName(), vertex->getRelatedParam(j)->getName());
		}

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < vertex->getNbParameters(); j++) {
			PiSDFParameter* param = vertex->getParameter(j);
			platform_fprintf("\t%s->%s [style=dotted];\n", param->getName(), vertex->getName());
		}
	}

	// Drawing PiSDF vertices.
	for (UINT32 i = 0; i < graph->getNb_pisdf_vertices(); i++) {
		PiSDFVertex *vertex = graph->getPiSDFVertex(i);
		draw_vertex(vertex, displayNames);
	}

	// Drawing Input vertices.
	for (UINT32 i = 0; i < graph->getNb_input_vertices(); i++) {
		draw_vertex(graph->getInput_vertex(i), displayNames);
	}

	// Drawing Output vertices.
	for (UINT32 i = 0; i < graph->getNb_output_vertices(); i++) {
		draw_vertex(graph->getOutput_vertex(i), displayNames);
	}

	// Drawing edges.
	for (UINT32 i=0 ; i<graph->getNb_edges(); i++)
	{
		char shortenedPExpr[EXPR_LEN_MAX];
		char shortenedCExpr[EXPR_LEN_MAX];
		PiSDFEdge* edge = graph->getEdge(i);

		globalParser.prettyPrint(edge->getProduction(), shortenedPExpr);
		globalParser.prettyPrint(edge->getConsumption(), shortenedCExpr);

		/*platform_fprintf ("\t%s->%s [taillabel=\"%s\" headlabel=\"%s\" labeldistance=%d labelangle=50];\n",
			edge->getSource()->getName(),edge->getSink()->getName(),
			shortenedPExpr,shortenedCExpr,labelDistance);*/
		platform_fprintf ("\t%s->%s [taillabel=\"(%d):%s\" headlabel=\"(%d):%s\"];\n",
			edge->getSource()->getName(),
			edge->getSink()->getName(),
			edge->getSource()->getOutputEdgeIx(edge),
			shortenedPExpr,
			edge->getSink()->getInputEdgeIx(edge),
			shortenedCExpr);
		//labelDistance = 3 + labelDistance%(3*4); // Oscillating the label distance to keep visibility
	}

	platform_fprintf ("}\n");
	platform_fclose();
}

void DotWriter::write(PiSDFAbstractVertex **schedulableVertices, UINT32 nbSchedulabeVertices, const char *path, char displayNames)
{
	//char directory[_MAX_PATH];
	//getcwd(directory, sizeof(directory));

	platform_fopen (path);
	// Writing header
	platform_fprintf ("digraph csdag {\n");
	platform_fprintf ("node [color=\"#433D63\"];\n");
	platform_fprintf ("edge [color=\"#9262B6\" arrowhead=\"empty\"];\n");

	for (UINT32 i = 0; i < nbSchedulabeVertices; i++) {
		// Drawing vertex.
		PiSDFAbstractVertex* vertex = schedulableVertices[i];
		draw_vertex(vertex, 1, false);

		// Replacing hierarchical vertices by their output child vertex.
		// TODO: ..for the input vertex.
		if(vertex->getType() == output_vertex)
		{
			for (UINT32 j = 0; j < ((PiSDFIfVertex*)vertex)->getParentVertex()->getNbOutputEdges(); j++) {
				PiSDFEdge* edge = ((PiSDFIfVertex*)vertex)->getParentVertex()->getOutputEdge(j);
				edge->setSource(vertex);

//								platform_fprintf ("\t%s->%s [taillabel=\"%d\" headlabel=\"%d\"];\n",
//									edge->getSource()->getName(),
//									vertex->getName(),
//									edge->getProductionInt(),
//									edge->getConsumptionInt());
			}
		}

		// Drawing edges.
		if(vertex->getType() == input_vertex)
		{
			for (UINT32 j = 0; j < ((PiSDFIfVertex*)vertex)->getParentVertex()->getNbInputEdges(); j++) {
							PiSDFEdge* edge = ((PiSDFIfVertex*)vertex)->getParentVertex()->getInputEdge(j);
							platform_fprintf ("\t%s->%s [taillabel=\"%d\" headlabel=\"%d\"];\n",
								edge->getSource()->getName(),
								vertex->getName(),
								edge->getProductionInt(),
								edge->getConsumptionInt());
			}
		}
		else
		{
			for (UINT32 j = 0; j < vertex->getNbInputEdges(); j++) {
				PiSDFEdge* edge = vertex->getInputEdge(j);
				if(edge->getConsumptionInt() > 0)

//					if(edge->getConsumptionInt() != edge->getDelayInt())
					platform_fprintf ("\t%s->%s [taillabel=\"%d\" headlabel=\"%d\"];\n",
							edge->getSource()->getName(),
							edge->getSink()->getName(),
							edge->getProductionInt(),
							edge->getConsumptionInt());
			}
		}
	}

	platform_fprintf ("}\n");

	platform_fclose();
}
