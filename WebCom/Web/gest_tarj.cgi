t <html><head>
t
t <script>
t   function habilitarEdicion() {
t     // Obtenemos todos los select e inputs del formulario
t     var form = document.forms['cgi'];
t     var elementos = form.querySelectorAll('select, input:not([type="button"])');
t     for (var i = 0; i < elementos.length; i++) {
t       elementos[i].disabled = false;
t     }
t     // Ocultamos el botón de editar y mostramos el de guardar
t     document.getElementById('btnEditar').style.display = 'none';
t     document.getElementById('btnGuardar').style.display = 'inline-block';
t   }
t </script>
t
t <style>
t  body, table, td, th, p, font, a {
t  color: white;
t  font-family: Georgia;
t  text-shadow: 2px 2px 4px rgba(0,0,0,1);
t }
t
t  table {
t   width: 70%;
t   border-collapse: collapse;
t   text-align: center;
t   margin-left: auto;
t   margin-right: auto;
t   table-layout: fixed;
t  }
t
t  th, td {
t   border: 1px solid white;
t   padding: 10px;
t   font-size: 25px;
t   width: 20%;
t  }
t
t  th {
t  background-color: rgba(255,255,255,0.2);
t  }
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
t
t </style><title>Gestión de tarjetas</title></head>
t <body text="white">
t <h2 align=center><br>GESTIÓN DE TARJETAS</h2>
t <p><br>Introduce la información del trabajador a la tarjeta asociada</p>
t <form action=gest_tarj.cgi method=post name=cgi>
t
t <table>
t  <tr>
t    <th>-</th>
t    <th>ID TARJETA</th>
t    <th>TAQUILLA</th>
t    <th>NOMBRE TRABAJADOR</th>
t  </tr>
t
c j row
t  </table>
t
t <div style="text-align:center; margin-top:20px;">
t
t  <input type="button" id="btnEditar" value="Editar" onclick="habilitarEdicion()" 
t  style="font-size:25px; padding:10px 20px; background-color: #4CAF50; color: white;">
t
t  <input type="submit" id="btnGuardar" value="Guardar Cambios" style="font-size:25px;
t  padding:10px 20px; display:none;"></div>
t
t  <input type="hidden" value="gest_tarj" name="pg">
t  </form></body></html>
.
