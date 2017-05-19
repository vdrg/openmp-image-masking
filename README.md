# Tarea 1

Para compilar, ejecutar `make`. El ejecutable generado se llama se llama masker. El código del programa se encuentra en el directorio "src". En "tests.py" (python3) se encuentran las pruebas que corrí, y en "results.csv" se encuentran los resultados.

# Uso

```
  $ ./masker [options] source.png result.png
  
  Options:
    -h, --help: Print help message.
    -n, --niter: Number of iterations. Default: 10
    -m, --mask: Path of the mask file.
    -s, --masksize: Size of the default mask (blur). If a mask file is specified, this option is ignored. Default: 3 (3x3)
    -t, --threads: Number of threads. Default: 1
      , --verbose: Log detailed information.
      , --no-output: Don't write the output image.
```

Para la funcionalidad extra de cargar máscaras custom, la primera línea del archivo debe ser el tamaño a utilizar (si la primera línea es 3, se usa una máscara de 3x3). Inclui un ejemplo en el directorio "masks".
