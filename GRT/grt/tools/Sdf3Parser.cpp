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

///*
// * Sdf3Parser.cpp
// *
// *  Created on: Jan 16, 2013
// *      Author: julien
// */
//
//#include "Sdf3Parser.h"
//
//#include <libxml/tree.h>
//#include <libxml/parser.h>
//#include <libxml/xmlstring.h>
//#include "SchedulingError.h"
//#include "../SchedulerDimensions.h"
//#include <cstdio>
//#include <cstring>
//
//// The parser object
//Sdf3Parser sdf3Parser;
//
//Sdf3Parser::Sdf3Parser() {
//}
//
//Sdf3Parser::~Sdf3Parser() {
//}
//
//xmlNodePtr getnode(xmlDocPtr doc, xmlNodePtr cur, const char* name){
//	cur = cur->xmlChildrenNode;
//	while (cur != NULL) {
//		if ((!xmlStrcmp((const xmlChar *)cur->name, (const xmlChar *)name))){
//			return cur;
//		}
//		cur = cur->next;
//	}
//	fprintf(stderr, "Cannot get %s\n", name);
//	exitWithCode(1045);
//	return 0;
//}
//
//xmlNodePtr getnodefromprop(xmlDocPtr doc, xmlNodePtr cur, const char* prop, const char* name){
//	cur = cur->xmlChildrenNode;
//	while (cur != NULL) {
//		if ((!xmlStrcmp((const xmlChar *)xmlGetProp(cur, (const xmlChar *)"name"), (const xmlChar *)name))){
//			return cur;
//		}
//		cur = cur->next;
//	}
//	fprintf(stderr, "Cannot get %s\n", name);
//	exitWithCode(1045);
//	return 0;
//}
//
//int Sdf3Parser::parse(const char* path, CSDAGGraph* graph, Architecture* archi){
//	xmlDocPtr doc; /* the resulting document tree */
//	xmlNodePtr cur, applicationNode;
//	CSDAGVertex* tabVertex[MAX_CSDAG_VERTICES];
//	xmlNodePtr tabVertexNode[MAX_CSDAG_VERTICES];
//
//
//	doc = xmlReadFile(path, NULL, 0);
//	if (doc == NULL) {
//		fprintf(stderr, "Failed to parse %s\n", path);
//		return -1;
//	}
//
//	cur = xmlDocGetRootElement(doc);
//	if (cur == NULL) {
//		fprintf(stderr,"empty xml\n");
//		xmlFreeDoc(doc);
//		return -1;
//	}
//
//	if (xmlStrcmp((const xmlChar *)cur->name, (const xmlChar *) "sdf3")) {
//		fprintf(stderr,"document of the wrong type, root node != applicationGraph (%s)", cur->name);
//		xmlFreeDoc(doc);
//		return -1;
//	}
//
//	applicationNode = cur = getnode(doc,cur,"applicationGraph");
//	cur = getnode(doc,cur,"sdf");
//
//	int i=0;
//	cur = cur->xmlChildrenNode;
//	while (cur != NULL) {
//		if ((!xmlStrcmp((const xmlChar *)cur->name, (const xmlChar *)"actor"))){
//			tabVertexNode[i] = cur;
//			tabVertex[i] = graph->addVertex((const char*) xmlGetProp(cur, (const xmlChar *)"name"));
//			tabVertex[i]->setFunctionIndex(i);
//			i++;
//		}
//		if ((!xmlStrcmp((const xmlChar *)cur->name, (const xmlChar *)"channel"))){
//			CSDAGVertex *src = NULL, *dst = NULL;
//			char cons[MAX_TIMING_EXPRESSION], prod[MAX_TIMING_EXPRESSION];
//
//			for(int j=0; j<i; j++){
//				if(strcmp(tabVertex[j]->getName(), (const char*) xmlGetProp(cur, (const xmlChar *)"srcActor"))==0){
//					src = tabVertex[j];
//					xmlNodePtr portNode = getnodefromprop(doc, tabVertexNode[j], "port", (const char*) xmlGetProp(cur, (const xmlChar *)"srcPort"));
//					strcpy(cons, (const char*) xmlGetProp(portNode, (const xmlChar *)"rate"));
//				}
//				if(strcmp(tabVertex[j]->getName(), (const char*) xmlGetProp(cur, (const xmlChar *)"dstActor"))==0){
//					dst = tabVertex[j];
//					xmlNodePtr portNode = getnodefromprop(doc, tabVertexNode[j], "port", (const char*) xmlGetProp(cur, (const xmlChar *)"dstPort"));
//					strcpy(prod, (const char*) xmlGetProp(portNode, (const xmlChar *)"rate"));
//				}
//			}
//			graph->addEdge(src, cons, dst, prod);
//		}
//		cur = cur->next;
//	}
//
//	cur = applicationNode; //applicationGraph
//
//	cur = getnode(doc,cur,"sdfProperties");
//
//	cur = cur->xmlChildrenNode;
//	i=0;
//	while (cur != NULL) {
//		if ((!xmlStrcmp((const xmlChar *)cur->name, (const xmlChar *)"actorProperties"))){
//			xmlNodePtr tmpNode = getnode(doc, cur, "processor");
//			tmpNode = getnode(doc, tmpNode, "executionTime");
//			tabVertex[i]->addTiming(0,(const char*) xmlGetProp(tmpNode, (const xmlChar *)"time")); // Not taken into account
//			tabVertex[i]->addAllConstraints(1, archi->getNbSlaves());
//			i++;
//		}
//		cur = cur->next;
//	}
//
//	xmlFreeDoc(doc);
//
//	return 0;
//}
