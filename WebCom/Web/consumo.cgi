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
t <title>ESTADO Y MODO</title></head>
t
t <body text="white">
t <div id="datos"><h2 align=center><br>CONFIGURACI&Oacute;N HORAS</h2>
t <p>Configuraci&oacute;n de las horas para dormir y despertar<br></p>
t
t <form action=consumo.cgi method=post name=cgi>
t
t <p><br>Introducir hora para dormir</p>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <td colspan="2">
t <input type="number" name="hora_dorm_per" min="0" max="23"
c m 1  value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="HH"> :
t
t <input type="number" name="min_dorm_per" min="0" max="59" 
c m 2 value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="MM"></td></div>
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
t
t <p><br>Introducir hora para despertar</p>
t
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
t
t <td colspan="2">
t <input type="number" name="hora_desp_per" min="0" max="23"
c m 3  value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="HH"> :
t
t <input type="number" name="min_desp_per" min="0" max="59" 
c m 4 value="%d" style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;" placeholder="MM"></td></div>
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
