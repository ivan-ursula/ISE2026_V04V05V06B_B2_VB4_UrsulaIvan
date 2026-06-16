t <html><head>
t
t <style>
t  body, td{
t  color: white;
t  font-family: Georgia;
t  text-shadow: 2px 2px 4px rgba(0,0,0,1);
t  font-size: 24px;
t  margin-top: 0px;
t  text-align: center;
t }
t
t  td {
t    font-size: 24px;
t    margin-top: 0px;
t    text-align: center;
t  }
t
t  .led-circle {
t    width: 50px;
t    height: 50px;
t    border-radius: 50%;
t    margin: 20px auto;
t    border: 2px solid #fff;
t    box-shadow: 0 0 10px rgba(0,0,0,0.5);
t    transition: background-color 0.5s ease;
t  }
t 
t  button {
t    font-size: 18px; border-radius: 8px; cursor: pointer;
t    margin: 5px; border: none; padding: 10px 20px;
t  }
t
t </style>
t
t <title>VISUALIZACIÓN DE DATOS</title></head>
t
t <body text="white">
t <div id="contenedor-dinamico">
t <form action=hora.cgi method=post name=cgi>
t
t <div class="led-circle" style="
c k 1 %s"></div>
c k 2 <p>%s</p>
t
t <div id="controles-alarma">
c l 0 %s
t </div>
t
t <div id="datos">
t
c d 1 <p> %s // 
c d 2  %s // 
c d 3  %s</p>
t </div></form></div>
t
t <script>
t function actualizarDatos() {
t  fetch("hora.cgi?nocache=" + new Date().getTime())
t     .then(response => response.text())
t     .then(data => {
t       var parser = new DOMParser();
t       var doc = parser.parseFromString(data, 'text/html');
t       document.getElementById("contenedor-dinamico").innerHTML = doc.getElementById("contenedor-dinamico").innerHTML;
t     });
t }
t
t setInterval(actualizarDatos, 1000);
t </script></body></html>
.
