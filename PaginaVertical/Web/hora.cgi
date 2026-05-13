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
t </style>
t
t <title>VISUALIZACIÓN DE DATOS</title></head>
t
t <body text="white">
t <div id="datos">
t <form action=hora.cgi method=post name=cgi>
t
c d 1 <td> %s // </td>
c d 2 <td> %s // </td>
c d 3 <td> %s</td>
t </form></div>
t
t <script>
t function actualizarDatos() {
t  fetch("hora.cgi?nocache=" + new Date().getTime())
t     .then(response => response.text())
t     .then(data => {
t       document.getElementById("datos").innerHTML = data;
t     });
t }
t
t setInterval(actualizarDatos, 1000);
t </script></body></html>
.
