t <html><head>
t <style>
t  body, table, td, th, p, font, a {
t  color: white;
t  font-family: Georgia;
t  text-shadow: 2px 2px 4px rgba(0,0,0,1);
t }
t table {
t  width: 70%;
t  border-collapse: collapse;
t  text-align: center;
t  margin-left: auto;
t  margin-right: auto;
t  table-layout: fixed;
t }
t th, td {
t  border: 1px solid white;
t  padding: 10px;
t  font-size: 25px;
t  width: 50%;
t }
t th {
t  background-color: rgba(255,255,255,0.2);
t }
t  body {
t    color: white;
t    font-size: 35px;
t  }
t  h2 {
t    font-size: 40px;
t    text-align: center;
t    margin-top: 30px;
t  }
t  p {
t    font-size: 29px;
t    margin-top: 50px;
t    text-align: center;
t  }
t </style><title>Alertas</title></head>
t <body text="white">
t <h2><br>ALERTAS</h2>
t <p>Registro de las alarmas del sistema</p>
t <table>
t  <tr>
t    <th>ALERTAS</th>
t  </tr>
t <form action=alertas.cgi method=post name=alertas>
c a 1 <tr><td>%s</td>
c a 2 <tr><td>%s</td>
c a 3 <tr><td>%s</td>
c a 4 <tr><td>%s</td>
c a 5 <tr><td>%s</td>
c a 6 <tr><td>%s</td>
c a 7 <tr><td>%s</td>
c a 8 <tr><td>%s</td>
c a 9 <tr><td>%s</td>
c a 10 <tr><td>%s</td>
t </table>
t </form>
t <form action=alertas.cgi method=post name=alertas>
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:20px; font-size:30px; gap:10px;">
c c 1 <td><input type=text name=pagina size=3 maxlength=3 value="%d" 
t style="text-align:center; font-size:30px; background: transparent; 
t outline: none; color: inherit; font: inherit; 
t text-shadow: inherit;"></td></div>
t </form>
t <form action=alertas.cgi method=post name=alertas>
t <div style="display:flex; justify-content:center; align-items:center; 
t margin-top:0px; font-size:30px; gap:10px;">
t <button type="submit" name="accion" value="prev" 
t style="background: transparent; border: none; outline: none; color: inherit;
t font: inherit; text-shadow: inherit; cursor: pointer;"><-</button>
t <button type="submit" name="accion" value="next" 
t style="background: transparent; border: none; outline: none; color: inherit;
t font: inherit; text-shadow: inherit; cursor: pointer;">-></button>
t </div>
t <input type=hidden value="gest_tarj" name=pg></form></body>
.
