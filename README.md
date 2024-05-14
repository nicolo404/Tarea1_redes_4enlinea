Grupo 6
Nicolás Javier Silva Pereira
Sebastián Henry Garrido Mariñán

##Conecta 4 - Cliente/Servidor

Este proyecto implementa el juego Conecta 4 mediante el modelo Cliente/Servidor en C++. El servidor maneja las conexiones de los clientes y permite que múltiples clientes jueguen simultáneamente. El juego sigue las reglas tradicionales de Conecta 4, donde dos jugadores se turnan para dejar caer fichas en una cuadrícula de 6 filas por 7 columnas.

#Instalación

    Clonar el repositorio: git clone https://github.com/nicolo404/Tarea1_redes_4enlinea.git

    Compilar el proyecto: instrucciones en archivo Makefile, para compilar tanto servidor como cliente


#Uso
    Servidor: Para iniciar el servidor, ejecute el siguiente comando:
    ./servidor <puerto>
    Ejemplo:./servidor 7777

    Cliente: Para conectar un cliente al servidor, ejecute el siguiente comando:
    ./cliente <ip_servidor> <puerto_servidor>
    Ejemplo:
    ./cliente 127.0.0.1 7777


