var secciones = [];
var volver_otro = false;

window.onload = function(){
    inicializarReferencias();
    var navbar = document.getElementById("navbar");
    var volver = document.getElementById("volver");
}

function inicializarReferencias(){
    secciones[1] = document.getElementById("login");
    secciones[3] = document.getElementById("inicio");
    secciones[4] = document.getElementById("perfil");
    secciones[5] = document.getElementById("plantas");
    secciones[6] = document.getElementById("planta");
    $(document).ready(function() {
            $('.contenedor_item_planta').on('click', function() {
                req = $.ajax({
                    url : '/mostrar',
                    type : 'POST',
                    data : {}
                });

                req.done(function(data) {
                    $('#tabla_fecha').text(data.sensorTime);
                    $('#tabla_temperatura').text(data.sensorTemperatura+' C°');
                    $('#tabla_humedad').text(data.sensorTierra+' %');
                    $('#tabla_luz').text(data.sensorLuz+' lx');
                    $('#tabla_humedadAmbiental').text(data.sensorHumedad+' %');
                    if(data.sensorTierra < 45){
                         alert("Riegue la planta");
                         $('.Estado').text('Regular');
                    }
                    else{
                         $('.Estado').text('Optimo');
                    }

                });


            });

        });
}
function cambiarSeccion(id_seccion){
    for (var i in secciones) {
        secciones[i].classList.add("oculto");
    }
    if(id_seccion == 3 || id_seccion == 1){
        volver.classList.add("oculto")
    }
    else{
        volver.classList.remove("oculto");
    }
    if(volver_otro){
        secciones[5].classList.remove("oculto");
        volver.classList.remove("oculto");
    }
    else{
        secciones[id_seccion].classList.remove("oculto");
    }
    if(id_seccion==6){
        volver_otro = true;
    }
    else{
        volver_otro = false;
    }
}
