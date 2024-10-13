import os
import shutil
import subprocess

def build_project():
    build_dir = "build"

    # Remove o diretório "build" se ele existir
    #if os.path.exists(build_dir):
    #    shutil.rmtree(build_dir)
    
    # Cria o diretório "build"
    os.makedirs(build_dir, exist_ok=True)

    # Muda para o diretório "build"
    os.chdir(build_dir)

    try:
        # Executa o comando `cmake ..`
        subprocess.run(["cmake", ".."], check=True)

        # Executa o comando `make`
        subprocess.run(["make"], check=True)
    finally:
        # Retorna ao diretório anterior
        os.chdir("..")

if __name__ == "__main__":
    build_project()
