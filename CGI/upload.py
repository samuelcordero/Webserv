#!/usr/bin/env python3

import cgi
import os

# Configura el directorio donde se guardarán los archivos subidos
UPLOAD_DIR = "/path/to/upload/directory"

def save_file(file_item):
    """Guarda el archivo en el directorio de subida."""
    if file_item.filename:
        filename = os.path.basename(file_item.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)
        with open(filepath, 'wb') as fout:
            while True:
                chunk = file_item.file.read(8192)
                if not chunk:
                    break
                fout.write(chunk)
        return filename
    return None

def main():
    # Inicializa el CGI
    form = cgi.FieldStorage()
    
    # Establece la cabecera de la respuesta
    print("Content-Type: text/html\n")

    # Si hay un archivo en la solicitud
    if "file" in form:
        file_item = form["file"]
        filename = save_file(file_item)

        if filename:
            print(f"<h1>Archivo subido exitosamente:</h1>")
            print(f"<p>Nombre del archivo: {filename}</p>")
        else:
            print("<h1>Error en la subida del archivo</h1>")
    else:
        print("<h1>No se ha recibido ningún archivo</h1>")

if __name__ == "__main__":
    main()
