#ifndef SPIDER_H
#define SPIDER_H

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <tools/StaticStack.h>

typedef struct{
	bool printSrdag;
	const char* srdagfile;
} SpiderConfig;

#include <graphTransfo/GraphTransfo.h>

#endif//SPIDER_H
