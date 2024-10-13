# Versão mínima do CMake
cmake_minimum_required(VERSION 3.12)

# Nome do projeto
project(projeto_raylib C)

# Definindo a versão C padrão
set(CMAKE_C_STANDARD 99)

# Adicionando o diretório da Raylib como subdiretório
add_subdirectory(deps/raylib)

# Incluindo diretório de headers do Raylib e do projeto
include_directories(deps/raylib/src includes)

# Adicionando o executável com os arquivos fonte
add_executable(projeto src/main.c)

# Ligando o executável à Raylib
target_link_libraries(projeto raylib)

# Para sistemas Unix, é necessário linkar com outras bibliotecas
if(UNIX)
    target_link_libraries(projeto m) # Linka com a math library
endif()

# Instrução opcional para copiar arquivos de assets ao diretório bin
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

# Definindo opções de compilação para modo Debug
if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(projeto PRIVATE -g)
endif()

# Adicionando flags específicas dependendo do sistema operacional
if (WIN32)
    add_compile_definitions(PLATFORM_DESKTOP)
elseif (UNIX)
    add_compile_definitions(PLATFORM_DESKTOP)
endif()

# Definindo a pasta de saída para o binário
set_target_properties(projeto PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
