t <html><head><meta charset="UTF-8">
t
t <style>
t  body, table, td, th, p, font, a {
t  color: white;
t  font-family: Georgia;
t  text-shadow: 2px 2px 4px rgba(0,0,0,1);
t }
t
t table {
t  width: 70%;
t  border-collapse: collapse;
t  text-align: center;
t  margin-left: auto;
t  margin-right: auto;
t  margin-top: 20px;
t  table-layout: fixed;
t }
t
t th, td {
t  border: 1px solid white;
t  padding: 10px;
t  font-size: 30px;
t  width: 50%;
t }
t
t th {
t  background-color: rgba(255,255,255,0.2);
t }
t
t  body {
t    color: white;
t    font-size: 35px;
t  }
t
t  h2 {
t    font-size: 40px;
t    text-align: center;
t    margin-top: 30px;
t  }
t
t  p {
t    font-size: 29px;
t    margin-top: 50px;
t    text-align: center;
t  }
t input[type="checkbox"] {
t   transform: scale(2);
t   cursor: pointer;
t }
t </style>
t
t <title>GESTIÓN DE TAQUILLAS</title></head>
t
t <body text="white">
t <div id="datos"><h2 align=center><br>GESTI&Oacute;N DE TAQUILLAS</h2>
t <p><br>Gesti&oacute;n e informaci&oacute;n de la apertura de las taquillas</p>
t
t <form action=datos.cgi method=post name=cgi>
t
t <table>
t  <tr>
t    <th>-</th>
t    <th>TAQUILLA 1</th>
t    <th>TAQUILLA 2</th>
t  </tr>
t  <tr>
t  <td>PESO</td>
c f 1 <td>%s</td>
c f 2 <td>%s</td>
t  </tr>
t  <tr>
t  <td>Estado de apertura</td>
t  <input type="hidden" name="taquilla1" value="0">
t  <input type="hidden" name="taquilla2" value="0">
c e 1 <td><input type="checkbox" name="taquilla1" OnClick="submit();" %s></td>
c e 2 <td><input type="checkbox" name="taquilla2" OnClick="submit();" %s></td>
t </tr></table></form></div>
t <script>
t function actualizarDatos() {
t  fetch("datos.cgi?nocache=" + new Date().getTime())
t     .then(response => response.text())
t     .then(data => {
t       document.getElementById("datos").innerHTML = data;
t     });
t }
t
t setInterval(actualizarDatos, 1000);
t </script></body></html>
.
