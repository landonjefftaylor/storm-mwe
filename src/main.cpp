#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <vector>
#include <queue>

#include <storm/api/storm.h>
#include <storm-parsers/api/storm-parsers.h>
#include <storm-parsers/parser/PrismParser.h>
#include <storm/storage/prism/Program.h>
#include <storm/storage/jani/Property.h>
#include <storm/storage/BitVectorHashMap.h>
#include <storm/generator/PrismNextStateGenerator.h>
#include <storm/utility/initialize.h>

typedef storm::storage::BitVector CompressedState;

int main(int argc, char ** argv) {

	std::string filename = argc >= 2 ? std::string(argv[1]) : "models/ModifiedYeastPolarization.sm";
	std::string propFilename = argc >= 3 ? std::string(argv[2]) : "models/ModifiedYeastPolarization.csl";
	int maxNumToExplore = argc >= 4 ? atoi(argv[3]) : 1000;

	storm::utility::setUp();
	storm::settings::initializeAll("main", "main");

	auto modelFile = std::make_shared<storm::prism::Program>(
		storm::parser::PrismParser::parse(filename, true)
	);

	auto labels = modelFile->getLabels();

	auto propertiesVector = std::make_shared<std::vector<storm::jani::Property>>(
		storm::api::parsePropertiesForPrismProgram(propFilename, *modelFile)
	);

	std::vector<std::shared_ptr<storm::logic::Formula const>> fv;
	for (auto & prop : *propertiesVector) {
		auto formula = prop.getFilter().getFormula();
		fv.push_back(formula);
	}

	storm::builder::BuilderOptions options(fv);

	auto generator = std::make_shared<storm::generator::PrismNextStateGenerator<double, uint32_t>>(
		*modelFile // Obviously, it has to be conscious of the model file
		, options // then, it also has to know about the options you set
	);

	// State::setVariableInformation(generator->getVariableInformation());

	storm::storage::BitVectorHashMap<uint32_t> stateStorage;

	std::deque<CompressedState> explorationQueue;
	
	uint32_t stateCnt = 0;
	

	auto const stateToIdCallback = std::function<uint32_t (const storm::generator::CompressedState &)>([&](const CompressedState & state) {

		auto startTime = std::chrono::high_resolution_clock::now();
		bool stateExists = stateStorage.contains(state);
		auto lookupTime = std::chrono::high_resolution_clock::now() - startTime;

		if (stateExists) {
			return stateStorage.getValue(state);
		}

		explorationQueue.push_back(state);

		uint32_t idx = stateCnt++;
	
		stateStorage.findOrAdd(state, idx);

		return idx;
	});

	auto initStateIndexes = generator->getInitialStates(stateToIdCallback);

	while (!explorationQueue.empty() && stateCnt <= maxNumToExplore) {
		auto curState = explorationQueue.front();
		explorationQueue.pop_front();
		generator->load(curState);

		auto behavior = generator->expand(stateToIdCallback);

		for (auto const & choice : behavior) {
			for (auto const & stateProbabilityPair : choice) {
				auto successorIdx        = stateProbabilityPair.first;
				auto propensityOrProbability = stateProbabilityPair.second;

			}
		}
	}

	return EXIT_SUCCESS;
}

