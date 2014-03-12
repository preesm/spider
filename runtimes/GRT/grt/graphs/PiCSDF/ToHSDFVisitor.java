package net.sf.dftools.algorithm.model.sdf.visitors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;

import net.sf.dftools.algorithm.demo.SDFAdapterDemo;
import net.sf.dftools.algorithm.generator.SDFRandomGraph;
import net.sf.dftools.algorithm.model.parameters.ExpressionValue;
import net.sf.dftools.algorithm.model.parameters.InvalidExpressionException;
import net.sf.dftools.algorithm.model.parameters.Variable;
import net.sf.dftools.algorithm.model.sdf.SDFAbstractVertex;
import net.sf.dftools.algorithm.model.sdf.SDFEdge;
import net.sf.dftools.algorithm.model.sdf.SDFGraph;
import net.sf.dftools.algorithm.model.sdf.SDFInterfaceVertex;
import net.sf.dftools.algorithm.model.sdf.SDFVertex;
import net.sf.dftools.algorithm.model.sdf.esdf.SDFBroadcastVertex;
import net.sf.dftools.algorithm.model.sdf.esdf.SDFForkVertex;
import net.sf.dftools.algorithm.model.sdf.esdf.SDFJoinVertex;
import net.sf.dftools.algorithm.model.sdf.esdf.SDFRoundBufferVertex;
import net.sf.dftools.algorithm.model.sdf.types.SDFExpressionEdgePropertyType;
import net.sf.dftools.algorithm.model.sdf.types.SDFIntEdgePropertyType;
import net.sf.dftools.algorithm.model.visitors.IGraphVisitor;
import net.sf.dftools.algorithm.model.visitors.SDF4JException;
import net.sf.dftools.algorithm.model.visitors.VisitorOutput;

/**
 * Visitor used to transform an SDF into a single-rate SDF (for all edges : prod
 * = cons)
 * 
 * @author jpiat
 * @author kdesnos
 * 
 */
public class ToHSDFVisitor implements
		IGraphVisitor<SDFGraph, SDFAbstractVertex, SDFEdge> {

	/**
	 * Test the visitor
	 * 
	 * @param args
	 * @throws InvalidExpressionException
	 */
	public static void main(String[] args) throws InvalidExpressionException {
		@SuppressWarnings("unused")
		int nbVertex = 3, minInDegree = 1, maxInDegree = 5, minOutDegree = 1, maxOutDegree = 2;

		// Creates a random SDF graph
		@SuppressWarnings("unused")
		int minrate = 1, maxrate = 4;
		@SuppressWarnings("unused")
		SDFRandomGraph test = new SDFRandomGraph();
		/*
		 * SDFGraph demoGraph = test.createRandomGraph(nbVertex, minInDegree,
		 * maxInDegree, minOutDegree, maxOutDegree, minrate, maxrate);
		 */
		SDFAdapterDemo applet = new SDFAdapterDemo();
		SDFGraph demoGraph = createTestComGraph();
		SDFAdapterDemo applet2 = new SDFAdapterDemo();
		ToHSDFVisitor visitor = new ToHSDFVisitor();
		try {
			demoGraph.accept(visitor);
			applet2.init(demoGraph);
			applet.init(visitor.getOutput());
		} catch (SDF4JException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private SDFGraph outputGraph;

	/**
	 * GIves this visitor output
	 * 
	 * @return The output of the visitor
	 */
	public SDFGraph getOutput() {
		return outputGraph;
	}

	/**
	 * This method adds the {@link SDFEdge}s to the output Single-Rate
	 * {@link SDFGraph}.
	 * 
	 * @param sdf
	 *            the input {@link SDFGraph}
	 * @param matchCopies
	 *            a {@link Map} that associates each {@link SDFVertex} of the
	 *            input {@link SDFGraph} to its corresponding {@link SDFVertex}
	 *            in the output Single-Rate {@link SDFGraph}.
	 * @param output
	 *            the output Single-Rate {@link SDFGraph} where the
	 *            {@link SDFVertex} have already been inserted by
	 *            {@link #transformsTop(SDFGraph, SDFGraph)}.
	 * @throws InvalidExpressionException
	 */
	private void linkVerticesTop(SDFGraph sdf,
			HashMap<SDFAbstractVertex, Vector<SDFAbstractVertex>> matchCopies,
			SDFGraph output) throws InvalidExpressionException {

		// Scan the edges of the input graph
		for (SDFEdge edge : sdf.edgeSet()) {
			// sdf.getEdgeSource(edge); -- Removed by kdesnos on the 2012.01.11
			// sdf.getEdgeTarget(edge); -- Removed by kdesnos on the 2012.01.11
			SDFInterfaceVertex inputVertex = null;
			SDFInterfaceVertex outputVertex = null;

			// Retrieve the duplicates of the source and target of the current
			// edge
			Vector<SDFAbstractVertex> sourceCopies = matchCopies.get(sdf
					.getEdgeSource(edge));
			Vector<SDFAbstractVertex> targetCopies = matchCopies.get(sdf
					.getEdgeTarget(edge));
			int nbDelays = edge.getDelay().intValue();

			// Total number of token exchanged (produced and consumed) for this
			// edge
			int totalNbTokens = edge.getCons().intValue() * targetCopies.size();

			// Absolute target is the targeted consumed token among the total
			// number of consumed/produced tokens
			int absoluteTarget = nbDelays;
			int absoluteSource = 0;

			// totProd is updated to store the number of token consumed by the
			// targets that are "satisfied" by the added edges.
			int totProd = 0;

			// Until all consumed token are "satisfied"
			while (totProd < (edge.getCons().intValue() * targetCopies.size())) {

				// Index of the currently processed sourceVertex among the
				// duplicates of the current edge source.
				int sourceIndex = (absoluteSource / edge.getProd().intValue())
						% sourceCopies.size();
				// targetIndex is used to know which duplicates of the target
				// will
				// be targeted by the currently indexed copy of the source.
				int targetIndex = (absoluteTarget / edge.getCons().intValue())
						% targetCopies.size();

				// sourceProd and targetCons are the number of token already
				// produced/consumed by the currently indexed source/target
				int sourceProd = absoluteSource % edge.getProd().intValue();
				int targetCons = absoluteTarget % edge.getCons().intValue();

				// rest is both the production and consumption rate on the
				// created edge.
				int rest = Math.min(edge.getProd().intValue() - sourceProd,
						edge.getCons().intValue() - targetCons);

				// This int represent the number of iteration separating the
				// currently indexed source and target (between which an edge is
				// added)
				// If this int is > to 0, this means that the added edge must
				// have
				// delays (with delay=prod=cons of the added edge).
				// With the previous example:
				// A_1 will target B_(1+targetIndex%3) = B_0 (with a delay of 1)
				// A_2 will target B_(2+targetIndex%3) = B_1 (with a delay of 1)
				// Warning, this integer division is not factorable
				int iterationDiff = absoluteTarget / totalNbTokens
						- absoluteSource / totalNbTokens;

				// Testing zone beginning
				// for inserting explode and implode vertices
				if (rest < edge.getProd().intValue()
						&& !(sourceCopies.get(sourceIndex) instanceof SDFForkVertex)
						&& !(sourceCopies.get(sourceIndex) instanceof SDFBroadcastVertex)) {
					// If an exlode must be added
					SDFAbstractVertex explodeVertex = new SDFForkVertex();
					output.addVertex(explodeVertex);
					SDFAbstractVertex originVertex = (SDFAbstractVertex) sourceCopies
							.get(sourceIndex);
					explodeVertex.setName("explode_" + originVertex.getName()
							+ "_" + edge.getSourceInterface().getName());

					// Replace the source vertex by the explode in the
					// sourceCopies list
					sourceCopies.set(sourceIndex, explodeVertex);

					// Add an edge between the source and the explode
					SDFEdge newEdge = output.addEdge(originVertex,
							explodeVertex);
					newEdge.setDelay(new SDFIntEdgePropertyType(0));
					newEdge.setProd(new SDFIntEdgePropertyType(edge.getProd()
							.intValue()));
					newEdge.setCons(new SDFIntEdgePropertyType(edge.getProd()
							.intValue()));
					newEdge.setDataType(edge.getDataType());
					newEdge.setSourceInterface(edge.getSourceInterface());
					newEdge.setTargetInterface(edge.getTargetInterface());
				}
				if (rest < edge.getCons().intValue()
						&& !(targetCopies.get(targetIndex) instanceof SDFJoinVertex)
						&& !(targetCopies.get(targetIndex) instanceof SDFRoundBufferVertex)) {
					// If an implode must be added
					SDFAbstractVertex implodeVertex = new SDFJoinVertex();
					output.addVertex(implodeVertex);
					SDFAbstractVertex originVertex = (SDFAbstractVertex) targetCopies
							.get(targetIndex);
					implodeVertex.setName("implode_" + originVertex.getName()
							+ "_" + edge.getTargetInterface().getName());

					// Replace the target vertex by the implode one in the
					// targetCopies List
					targetCopies.set(targetIndex, implodeVertex);

					// Add an edge between the implode and the target
					SDFEdge newEdge = output.addEdge(implodeVertex,
							originVertex);
					newEdge.setDelay(new SDFIntEdgePropertyType(0));
					newEdge.setProd(new SDFIntEdgePropertyType(edge.getCons()
							.intValue()));
					newEdge.setCons(new SDFIntEdgePropertyType(edge.getCons()
							.intValue()));
					newEdge.setDataType(edge.getDataType());
					newEdge.setSourceInterface(edge.getSourceInterface());
					newEdge.setTargetInterface(edge.getTargetInterface());
				}
				// end of testing zone

				// Create the new Edge for the output graph
				SDFEdge newEdge = output.addEdge(sourceCopies.get(sourceIndex),
						targetCopies.get(targetIndex));

				// Set the source interface of the new edge
				if (sourceCopies.get(sourceIndex).getSink(
						edge.getSourceInterface().getName()) != null) {
					// if the source already has the appropriate interface
					newEdge.setSourceInterface(sourceCopies.get(sourceIndex)
							.getSink(edge.getSourceInterface().getName()));
				} else {
					// if the source does not have the interface.
					newEdge.setSourceInterface(edge.getSourceInterface()
							.clone());
				}

				// Set the target interface of the new edge
				if (targetCopies.get(targetIndex).getSource(
						edge.getTargetInterface().getName()) != null) {
					// if the target already has the appropriate interface
					newEdge.setTargetInterface(targetCopies.get(targetIndex)
							.getSource(edge.getTargetInterface().getName()));
				} else {
					// if the target does not have the interface.
					newEdge.setTargetInterface(edge.getTargetInterface()
							.clone());
				}
				// kdesnos: This lines cancel the previous if..else block ?
				newEdge.setTargetInterface(edge.getTargetInterface().clone());

				// Associate the interfaces to the new edge
				if (targetCopies.get(targetIndex) instanceof SDFVertex) {
					if (((SDFVertex) targetCopies.get(targetIndex))
							.getAssociatedInterface(edge) != null) {
						inputVertex = ((SDFVertex) targetCopies
								.get(targetIndex)).getAssociatedInterface(edge);
						((SDFVertex) targetCopies.get(targetIndex))
								.setInterfaceVertexExternalLink(newEdge,
										inputVertex);
					}
				}
				if (sourceCopies.get(sourceIndex) instanceof SDFVertex) {
					if (((SDFVertex) sourceCopies.get(sourceIndex))
							.getAssociatedInterface(edge) != null) {
						outputVertex = ((SDFVertex) sourceCopies
								.get(sourceIndex)).getAssociatedInterface(edge);
						((SDFVertex) sourceCopies.get(sourceIndex))
								.setInterfaceVertexExternalLink(newEdge,
										outputVertex);
					}
				}

				// Set the properties of the new edge
				newEdge.copyProperties(edge);
				newEdge.setProd(new SDFIntEdgePropertyType(rest));
				newEdge.setCons(new SDFIntEdgePropertyType(rest));

				// If the edge has a delay and that delay still exist in the
				// SRSDF (i.e. if the source & target do not belong to the same
				// "iteration")
				if (iterationDiff > 0) {
					int addedDelays = (iterationDiff * newEdge.getCons()
							.intValue());
					// Check that there are enough delays available
					if (nbDelays < addedDelays) {
						// kdesnos: I added this check, but it will most
						// probably never happen
						throw new RuntimeException(
								"Insufficient delays on edge "
										+ edge.getSource().getName() + "."
										+ edge.getSourceInterface().getName()
										+ "=>" + edge.getTarget().getName()
										+ "."
										+ edge.getTargetInterface().getName()
										+ ". At least " + addedDelays
										+ " delays missing.");
					}
					newEdge.setDelay(new SDFIntEdgePropertyType(addedDelays));
					nbDelays = nbDelays - addedDelays;
				} else {
					newEdge.setDelay(new SDFIntEdgePropertyType(0));
				}

				// Update the number of token produced/consumed by the currently
				// indexed source/target
				absoluteTarget += rest;
				absoluteSource += rest;

				// Update the totProd for the current edge (totProd is used in
				// the condition of the While loop)
				totProd += rest;

				// In case of a round buffer
				// If all needed tokens were already produced
				// but not all tokens were produced (i.e. not all source copies
				// were considered yet)
				if ((totProd == (edge.getCons().intValue() * targetCopies
						.size()))
						&& targetCopies.get(0) instanceof SDFInterfaceVertex
						&& (absoluteSource / edge.getProd().intValue()) < sourceCopies
								.size()) {
					totProd = 0;
				}
			}
			for (int i = 0; i < sourceCopies.size(); i++) {
				if (sourceCopies.get(i) instanceof SDFForkVertex
						&& !sourceCopies.get(i).equals(edge.getSource())) {
					SDFAbstractVertex trueSource = null;
					for (SDFEdge inEdge : output.incomingEdgesOf(sourceCopies
							.get(i))) {
						trueSource = inEdge.getSource();
					}
					sourceCopies.set(i, trueSource);
				}
			}
			for (int i = 0; i < targetCopies.size(); i++) {
				if (targetCopies.get(i) instanceof SDFJoinVertex
						&& !targetCopies.get(i).equals(edge.getTarget())) {
					SDFAbstractVertex trueTarget = null;
					for (SDFEdge inEdge : output.outgoingEdgesOf(targetCopies
							.get(i))) {
						trueTarget = inEdge.getTarget();
					}
					targetCopies.set(i, trueTarget);
				}
			}
		}
	}

	/**
	 * This method transforms a schedulable {@link SDFGraph} into its equivalent
	 * Single-Rate {@link SDFGraph}. The method duplicate the vertices according
	 * to the Repetition Vector of the {@link SDFGraph} then create the
	 * appropriate {@link SDFEdge}s through a call to
	 * {@link #linkVerticesTop(SDFGraph, HashMap, SDFGraph)}.
	 * 
	 * @param graph
	 *            the input {@link SDFGraph}
	 * @param output
	 *            the Single-Rate output {@link SDFGraph}
	 * @throws SDF4JException
	 * @throws InvalidExpressionException
	 */
	private void transformsTop(SDFGraph graph, SDFGraph output)
			throws SDF4JException, InvalidExpressionException {
		// This map associates each vertex of the input graph to corresponding
		// instances in the output graph
		HashMap<SDFAbstractVertex, Vector<SDFAbstractVertex>> matchCopies = new HashMap<SDFAbstractVertex, Vector<SDFAbstractVertex>>();

		if (graph.isSchedulable()) {
			// Scan the vertices of the input graph
			for (SDFAbstractVertex vertex : graph.vertexSet()) {
				Vector<SDFAbstractVertex> copies = new Vector<SDFAbstractVertex>();
				matchCopies.put(vertex, copies);

				// If the vertex is an interface, it will not be duplicated,
				// simply copy it in the output graph
				if (vertex instanceof SDFInterfaceVertex) {
					SDFAbstractVertex copy = ((SDFAbstractVertex) vertex)
							.clone();
					copies.add(copy);
					output.addVertex(copy);
				} else {
					// If the vertex is not an interface, duplicate it as many
					// times as needed to obtain single rates edges
					VisitorOutput.getLogger().log(Level.INFO,
							vertex.getName() + " x" + vertex.getNbRepeat());
					// If the vertex does not need to be duplicated
					if (vertex.getNbRepeatAsInteger() == 1) {
						SDFAbstractVertex copy = ((SDFAbstractVertex) vertex)
								.clone();
						copy.setName(copy.getName());
						output.addVertex(copy);
						copies.add(copy);
					} else {
						// If the vertex needs to be duplicated
						for (int i = 0; i < vertex.getNbRepeatAsInteger(); i++) {
							SDFAbstractVertex copy = ((SDFAbstractVertex) vertex)
									.clone();
							copy.setName(copy.getName() + "_" + i);
							copy.setNbRepeat(1);
							output.addVertex(copy);
							copies.add(copy);
						}
					}

				}
			}
			// The output graph has all its vertices, now deal with the edges
			linkVerticesTop(graph, matchCopies, output);
			output.getPropertyBean().setValue("schedulable", true);
		} else {
			VisitorOutput.getLogger().log(Level.SEVERE,
					"graph " + graph.getName() + " is not schedulable");
			throw (new SDF4JException("Graph " + graph.getName()
					+ " is not schedulable"));
		}
	}

	public void visit(SDFEdge sdfEdge) {
	}

	public void visit(SDFGraph sdf) throws SDF4JException {
		outputGraph = sdf.clone();
		boolean isHSDF = true;
		for (SDFAbstractVertex vertex : outputGraph.vertexSet()) {
			try {
				if (vertex instanceof SDFVertex
						&& vertex.getNbRepeatAsInteger() > 1) {
					isHSDF = false;
					break;
				}
			} catch (InvalidExpressionException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				throw (new SDF4JException(e.getMessage()));
			}
		}
		if (isHSDF) {
			return;
		}
		outputGraph.clean();

		ArrayList<SDFAbstractVertex> vertices = new ArrayList<SDFAbstractVertex>(
				sdf.vertexSet());
		for (int i = 0; i < vertices.size(); i++) {
			if (vertices.get(i) instanceof SDFVertex) {
				vertices.get(i).accept(this);
			}
		}
		try {
			transformsTop(sdf, outputGraph);
		} catch (InvalidExpressionException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw (new SDF4JException(e.getMessage()));
		}
	}

	public void visit(SDFAbstractVertex sdfVertex) {
		/*
		 * if(sdfVertex.getGraphDescription() != null){
		 * sdfVertex.getGraphDescription().accept(this); }
		 */
	}

	private static SDFGraph createTestComGraph() {

		SDFGraph graph = new SDFGraph();

		// test_com_basique
		SDFVertex sensorInt = new SDFVertex();
		sensorInt.setName("sensor_Int");
		graph.addVertex(sensorInt);

		SDFVertex gen5 = new SDFVertex();
		gen5.setName("Gen5");
		graph.addVertex(gen5);

		SDFVertex recopie5 = new SDFVertex();
		recopie5.setName("recopie_5");
		graph.addVertex(recopie5);

		SDFVertex acqData = new SDFVertex();
		acqData.setName("acq_data");
		graph.addVertex(acqData);

		SDFEdge sensGen = graph.addEdge(sensorInt, gen5);
		// sensGen.setTargetInterface(add);
		sensGen.setProd(new SDFIntEdgePropertyType(1));
		sensGen.setCons(new SDFIntEdgePropertyType(1));

		SDFEdge genRec = graph.addEdge(gen5, recopie5);
		// genRec.setSourceInterface(times);
		genRec.setProd(new SDFExpressionEdgePropertyType(new ExpressionValue(
				"SIZE")));
		genRec.setCons(new SDFExpressionEdgePropertyType(new ExpressionValue(
				"1+2")));

		SDFEdge genAcq = graph.addEdge(gen5, acqData);
		// genAcq.setSourceInterface(times);
		genAcq.setProd(new SDFIntEdgePropertyType(1));
		genAcq.setCons(new SDFIntEdgePropertyType(1));

		SDFEdge recAcq = graph.addEdgeWithInterfaces(recopie5, acqData);
		recAcq.setProd(new SDFIntEdgePropertyType(3));
		recAcq.setCons(new SDFIntEdgePropertyType(2));

		graph.addVariable(new Variable("a", "5"));
		graph.addVariable(new Variable("b", "10"));

		return graph;
	}

}