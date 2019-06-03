cmake_minimum_required(VERSION 3.5)
project (xcsrlib)

set(HEADER
    ${PROJECT_SOURCE_DIR}/include/xxr/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/random.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/block_world_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/checkerboard_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/csv_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/dataset_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/multiplexer_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/environment/real_multiplexer_environment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/helper/csv.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/helper/dataset.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/action_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/classifier.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/classifier_ptr_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/condition.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/constants.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/ga.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/match_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/population.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/prediction_array.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcs/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/action_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/classifier.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/condition.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/constants.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/repr.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/csr/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/csr/ga.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/csr/match_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/csr/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/obr/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/obr/ga.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/obr/match_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/obr/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/ubr/experiment.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/ubr/ga.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/ubr/match_set.hpp
    ${PROJECT_SOURCE_DIR}/include/xxr/xcsr/ubr/symbol.hpp
    )
    
set(SRC 
    ${PROJECT_SOURCE_DIR}/src/xcs.cpp
    ${PROJECT_SOURCE_DIR}/src/xcsr.cpp
    ${PROJECT_SOURCE_DIR}/src/common.hpp
    ${PROJECT_SOURCE_DIR}/src/unit_test/checkerboard_environment_test.cpp
    ${PROJECT_SOURCE_DIR}/src/unit_test/unit_test.hpp
    ${PROJECT_SOURCE_DIR}/src/util/simple_moving_average.hpp
    )

	
add_subdirectory("${PROJECT_SOURCE_DIR}/src/third_party/cxxopts/")
add_library(${PROJECT_NAME} STATIC ${HEADER}
								   ${SRC})
include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)			
target_include_directories(${PROJECT_NAME} 	PUBLIC ${PROJECT_SOURCE_DIR}/include/)		   