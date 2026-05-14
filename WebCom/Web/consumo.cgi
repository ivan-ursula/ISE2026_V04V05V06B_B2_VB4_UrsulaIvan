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
t <title>ESTADO Y CONSUMO</title></head>
t
t <body text="white">
t <div id="datos"><h2 align=center><br>ESTADO Y CONSUMO</h2>
t <p>Estado actual del sistema y opciones de consumo<br></p>
t
t <form action=consumo.cgi method=post name=cgi>
t
c g 1 <p>El sistema se encuentra en modo: %s</p>
t
t <p>Elegir modo de consumo</p>
t <table>
t  <tr>
t    <th>ACTIVO</th>
t    <th>STOP</th>
t  </tr>
t  <tr>
c h 1 <td><input type="radio" name="modo" value="1" OnClick="submit();" %s></td>
c h 2 <td><input type="radio" name="modo" value="2" OnClick="submit();" %s></td>
t  </tr></table>
t
t <p><br>Introducir fecha y hora para entrar al modo sleep</p>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <td colspan="2">
t <input type="number" name="hora_dorm" min="0" max="23"
c i 1  value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="HH"> :
t
t <input type="number" name="min_dorm" min="0" max="59" 
c i 2 value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="MM"></td>
t
t <input type="date" name="fecha_dorm"  
c i 3 min="%s"
c i 4 value="%s"
t style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; text-shadow: inherit;"></div>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <tr>
t   <td colspan="2">
t     <input type="submit" value="Guardar" style="text-align:center; font-size:30px; background: transparent; 
t 	  outline: none; color: inherit; font: inherit; 
t     text-shadow: inherit;">
t   </td>
t </tr></div>
t
t <p><br>Introducir fecha y hora para salir del modo sleep</p>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <td colspan="2">
t <input type="number" name="hora_desp" min="0" max="23"
c i 5  value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="HH"> :
t
t <input type="number" name="min_desp" min="0" max="59" 
c i 6 value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="MM"></td>
t
t <input type="date" name="fecha_desp" 
c i 7 min="%s"
c i 8 value="%s"
t style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; text-shadow: inherit;"></div>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <tr>
t   <td colspan="2">
t     <input type="submit" value="Guardar" style="text-align:center; font-size:30px; background: transparent; 
t     outline: none; color: inherit; font: inherit; 
t     text-shadow: inherit;">
t   </td>
t </tr></div>
t
t </form></div></body></html>
.
