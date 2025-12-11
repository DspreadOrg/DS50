cmake_minimum_required(VERSION 3.13)
add_definitions(-w)
set(SOURCE_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_TOP_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1) 

set(out_hex_dir ${CMAKE_CURRENT_BINARY_DIR}/hex)
set(out_lib_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(out_inc_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
set(tools_dir ${CMAKE_CURRENT_SOURCE_DIR}/tools)

set(core_stub_o ${CMAKE_CURRENT_SOURCE_DIR}/components/lib/core_stub.o)

set(flash_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_flashimg.ld)
set(file_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_fileimg.ld)

set(cmd_mkappimg dtools mkappimg)
set(pacgen_py ${tools_dir}/pacgen.py)
set(modemgen_py ${tools_dir}/modemgen.py)

include(cmake/core_config.cmake)
include(cmake/toolchain-gcc.cmake)
set(BUILD_SHARED_LIBS OFF)

project(core C CXX ASM)

set(COMPONENTS_PATH components)

include_directories(${COMPONENTS_PATH}/include ${COMPONENTS_PATH}/newlib/include)
include_directories(dspread/inc)
include_directories(app/inc)
include_directories(dspread/lvgl_v7) 
include_directories(dspread) 


  
function(cpp_only target file)
    add_library(${target} OBJECT ${file})
    set_source_files_properties(${file} PROPERTIES LANGUAGE C)
    target_compile_options(${target} PRIVATE -E -P -x c)
endfunction()

function(add_subdirectory_if_exist dir)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
            add_subdirectory(${dir})
        endif()
    endif()
endfunction()

function(add_appimg target ldscript)
    set(gen_ldscript ${target}_ldscript)
    set(target_map_file ${out_hex_dir}/${target}.map)
    set(target_img_file ${out_hex_dir}/${target}.img)
    cpp_only(${gen_ldscript} ${ldscript})
    add_executable(${target} ${ARGN} ${core_stub_o})
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${out_hex_dir})
    target_link_libraries(${target} PRIVATE -T $<TARGET_OBJECTS:${gen_ldscript}>)
    target_link_libraries(${target} PRIVATE -Wl,-Map=${target_map_file} -nostdlib -Wl,--gc-sections)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${cmd_mkappimg} $<TARGET_FILE:${target}> ${target_img_file}
        BYPRODUCTS ${target_img_file} ${target_map_file}
    )  
endfunction()

configure_file(${SOURCE_TOP_DIR}/components/lib/fdl1.img ${out_hex_dir}/fdl1.img COPYONLY)
configure_file(${SOURCE_TOP_DIR}/components/lib/fdl2.img ${out_hex_dir}/fdl2.img COPYONLY)

macro(pac_init_fdl cmd)
    set(${cmd}
        cfg-init --pname UIX8910_MODEM --palias "APPIMG"
            --pversion "8910 MODULE" --version "BP_R1.0.0"
            --flashtype 1
        cfg-host-fdl -a ${CONFIG_FDL1_IMAGE_START} -s ${CONFIG_FDL1_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl1.img
        cfg-fdl2 -a ${CONFIG_FDL2_IMAGE_START} -s ${CONFIG_FDL2_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl2.img
    )
endmacro()

if(CONFIG_APPIMG_LOAD_FLASH)
    set(target appimg_flash_delete)
    set(pac_file ${out_hex_dir}/${target}.pac)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-erase-flash -i ERASE_APPIMG
                -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()
  

file(GLOB LIB_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/libDS50.a)
file(GLOB LIB_EMV_KERNEL_INS_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/libEmvKernelIns.a)

file(GLOB LIB_EMV_KERNEL_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libkernel.a)
file(GLOB LIB_EMV_L2_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libemvl2.a)
file(GLOB LIB_EMV_ENTRYPOINT_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libentrypoint.a)
file(GLOB LIB_EMV_PAYPASS_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libpaypass.a)
file(GLOB LIB_EMV_PAYWAVE_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libpaywave.a)
file(GLOB LIB_EMV_DPAS_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libdpas.a)
file(GLOB LIB_EMV_PURE_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libpure.a)
file(GLOB LIB_EMV_AMEX_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libamex.a)
file(GLOB LIB_EMV_QUICS_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libquics.a)
file(GLOB LIB_EMV_JCB_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libjcb.a)
file(GLOB LIB_EMV_INTERAC_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libinterac.a)
file(GLOB LIB_EMV_BANCOMAT_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libbancomat.a)
file(GLOB LIB_EMV_RUPAY_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/librupay.a)
file(GLOB LIB_EMV_MIR_FILE_NAME ${CMAKE_SOURCE_DIR}/dspread/emv/libmir.a)


file(GLOB MAIN_SOURCES_FILE ${CMAKE_SOURCE_DIR}/app/src/*.c)
file(GLOB PUB_SOURCES_FILE ${CMAKE_SOURCE_DIR}/app/src/pub/*.c)
file(GLOB APP_SOURCES_FILE ${CMAKE_SOURCE_DIR}/app/src/app/*.c)

file(GLOB EMQX_SOURCES_FILE ${CMAKE_SOURCE_DIR}/app/src/emqx/*.c)

set(sign_password 12345678)     # customer product shall replace with customer's key
set(sign_product test)          # customer product shall replace with customer's product name
function(sign_image src dst)
    add_custom_command(OUTPUT ${dst}
        COMMAND vlrsign --pw ${sign_password} --pn ${sign_product} --ha Blake2
            --img ${src} --out ${dst}
        DEPENDS ${src}
    )
endfunction()



if(CONFIG_APPIMG_LOAD_FLASH)
    set(target S20_app)
    add_appimg(${target} ${flash_ldscript}
		${MAIN_SOURCES_FILE}
		${PUB_SOURCES_FILE}
		${APP_SOURCES_FILE}
        ${EMQX_SOURCES_FILE}
		)
    target_link_libraries(${target} PRIVATE -Wl,--start-group ${libc_file_name} ${libm_file_name} ${libgcc_file_name} ${LIB_FILE_NAME} ${LIB_EMV_KERNEL_INS_FILE_NAME} ${LIB_EMV_AMEX_FILE_NAME} ${LIB_EMV_DPAS_FILE_NAME} ${LIB_EMV_L2_FILE_NAME} ${LIB_EMV_ENTRYPOINT_FILE_NAME}  ${LIB_EMV_KERNEL_FILE_NAME} ${LIB_EMV_PAYPASS_FILE_NAME} ${LIB_EMV_PAYWAVE_FILE_NAME} ${LIB_EMV_PURE_FILE_NAME} ${LIB_EMV_RUPAY_FILE_NAME} -Wl,--end-group)

    set(prepack_cpio ${out_hex_dir}/${target}_prepack.cpio)
    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.img)
    pac_init_fdl(init_fdl)
         add_custom_command(OUTPUT ${prepack_cpio}
             COMMAND python3 ${modemgen_py} prepackgen
                 --source-top ${SOURCE_TOP_DIR}
                --binary-top ${BINARY_TOP_DIR}
                 --prepackfile ${CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH}
                 ${prepack_cpio}
             DEPENDS ${modemgen_py}
             WORKING_DIRECTORY ${SOURCE_TOP_DIR}
     )
   
    if(CONFIG_PACKAGE_FILE_APPIMG_JSON_PATH)
        set(cfg_pack_gpio cfg-pack-cpio -i PREPACK -p ${prepack_cpio})
        set(prepack_file ${prepack_cpio})
    endif()
     add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
             cfg-image -i APPIMG -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                 -s ${CONFIG_APPIMG_FLASH_SIZE}
                 -p ${target_img_file}
             ${cfg_pack_gpio}
            pac-gen ${pac_file}
         DEPENDS ${pacgen_py} ${target_img_file} ${prepack_file}
         WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
    
endif()
 