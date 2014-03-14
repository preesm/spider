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
