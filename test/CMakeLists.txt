#make everything into an executable
#using it's name as the base
#need to re-run cmake if adding new files

FILE(GLOB files *.cpp)


foreach(file ${files})
	get_filename_component(base_name ${file} NAME_WE)
#	message("found file for library " ${base_name})
	add_executable(${base_name} ${file} )
	target_link_libraries(${base_name} ${BOOST_LIBRARIES})
endforeach()
