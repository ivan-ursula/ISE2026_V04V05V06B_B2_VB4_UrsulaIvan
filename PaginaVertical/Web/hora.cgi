t <html><head><title>Hour-Date</title>
t <meta http-equiv="refresh" content="1"></head>
i pg_header.inc
t <h1 style="text-align: center;"><br>Hour-Date</h1>
t <p style="text-align: center;">&nbsp;</p>
t <form action=hora.cgi method=post name=cgi>
t <input type=hidden value="hora" name=pg>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <div style="text-align:center; font-size:20px;">
t <input type="text" readonly style="background-color: transparent; border: 0px; font-size:30px;"
c z 1 size="10" id="buf_hour" value="       %s">
t <br>
t <input type="text" readonly style="background-color: transparent; border: 0px; font-size:30px;"
c z 2 size="10" id="buf_date" value="    %s">
t </div>
t </p></form>
. End of script must be closed with period
