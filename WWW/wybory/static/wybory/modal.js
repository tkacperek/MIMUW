

var modal_data = new Object();

function getCookie(name) {
    var cookieValue = null;
    if (document.cookie && document.cookie != '') {
        var cookies = document.cookie.split(';');
        for (var i = 0; i < cookies.length; i++) {
            var cookie = jQuery.trim(cookies[i]);
            if (cookie.substring(0, name.length + 1) == (name + '=')) {
                cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
                break;
            }
        }
    }
    return cookieValue;
}

function csrfSafeMethod(method) {
    return (/^(GET|HEAD|OPTIONS|TRACE)$/.test(method));
}

function modal_start(woj, typ, gte, lte) {

    var csrftoken = getCookie('csrftoken');

    $.ajaxSetup({
        beforeSend: function(xhr, settings) {
            if (!csrfSafeMethod(settings.type) && !this.crossDomain) {
                xhr.setRequestHeader("X-CSRFToken", csrftoken);
            }
        }
    });

    $.post("/ajax/", {
        woj: woj,
        typ: typ,
        gte: gte,
        lte: lte
    }, function(data) {

        // var items = [];
        var html = '';

        $.each(data, function(key, obj) {
            // items.push(obj.fields);

            modal_data["" + obj.pk] = obj;

            var g1,g2;
            var name1, name2;
            g1 = obj.fields.l_glosow_1;
            g2 = obj.fields.l_glosow_2;
            name1 = "l_glosow_1";
            name2 = "l_glosow_2";
            if (flip == 1) {
                var tmp = g1;
                g1 = g2;
                g2 = tmp;

                tmp = name1;
                name1 = name2;
                name2 = tmp;
            }

            html += '<tr id="mtr' + obj.pk + '"><td style="text-align: left; white-space: nowrap;">' + obj.fields.nazwa + '</td>' +
                '<td class="data" name="' + name1 + '">' + g1 + '</td>' +
                '<td class="data"name="' + name2 + '">' + g2 + '</td>' +
                '<td class="data" name="l_mieszkancow">' + obj.fields.l_mieszkancow + '</td>' +
                '<td class="data" name="l_uprawnionych">' + obj.fields.l_uprawnionych + '</td>' +
                '<td class="data" name="l_wydanych_kart">' + obj.fields.l_wydanych_kart + '</td>' +
                '<td class="data" name="l_oddanych_glosow">' + obj.fields.l_oddanych_glosow + '</td>' +
                '<td class="data" name="l_waznych_glosow">' + obj.fields.l_waznych_glosow + '</td>' +
                '<input type="hidden" name="force" value="0">';

            if (login) {
                html += '<td style="white-space: nowrap;">' +
                    '<button type="button" class="btn btn-primary" onclick="on_edit(' + obj.pk + ');">Edytuj</button>' +
                    '</td>';
            }

            html += '</tr>';
        });
        $('#modal_content').html(html);

    }, "json");

    $('#myModal').modal();
}

function on_edit(pk) {
    var id = "mtr" + pk;

    $("#"+id+" .data").each(function() {
        var html = $(this).html();
        var input = $('<input type="text" class="form-control">');
        input.val(html);
        $(this).html(input);
    });

    var btn1 = '<button type="button" class="btn btn-primary" onclick="save(' + pk + ');">Zapisz</button>';
    $("#" + id + " td:last").html(btn1);
}
function save(pk) {
    var id = "#mtr" + pk;

    $(id + " .data").each(function(key, obj) {
        modal_data["" + pk].fields[$(obj).attr('name')] = Number($(obj.firstChild).val());
    });

    if (
        modal_data[pk].fields['l_mieszkancow'] < modal_data[pk].fields['l_uprawnionych'] ||
            modal_data[pk].fields['l_uprawnionych'] < modal_data[pk].fields['l_wydanych_kart'] ||
            modal_data[pk].fields['l_wydanych_kart'] < modal_data[pk].fields['l_oddanych_glosow'] ||
            modal_data[pk].fields['l_oddanych_glosow'] < modal_data[pk].fields['l_waznych_glosow'] ||
            modal_data[pk].fields['l_waznych_glosow'] < modal_data[pk].fields['l_glosow_1'] ||
            modal_data[pk].fields['l_waznych_glosow'] < modal_data[pk].fields['l_glosow_2'] ||
            modal_data[pk].fields['l_glosow_1'] + modal_data[pk].fields['l_glosow_2'] > modal_data[pk].fields['l_waznych_glosow']
    ) {
        var alert = $('<div id="alert" class="alert alert-danger alert-dismissible" style="position: fixed; top: 1%; left: 5%; width: 90%;">');
            alert.html('<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>' + 'Niepoprawne dane');

            $('#myModal').append(alert);

            $("#alert").fadeTo(2500, 500).slideUp(500, function(){
                $("#alert").alert('close');
            });

            return 0;
    }

    $.post("/modal_save/", {
        data: '[' + JSON.stringify(modal_data[pk]) + ']',
        force: $(id+" input[name=force]").attr('value')
    }, function(data) {

        if (data.r == 0) {
            $(id+" .data").each(function() {
                var val = $(this).children()[0].value
                $(this).html(val);
            });
            $($(id+" td:last").children()[0]).attr('onclick', 'on_edit('+pk+')');
            $($(id+" td:last").children()[0]).attr('class', 'btn btn-primary');
            $($(id+" td:last").children()[0]).html('Edytuj');

            var alert = $('<div id="alert" class="alert alert-success alert-dismissible" style="position: fixed; top: 1%; left: 5%; width: 90%;">');
            alert.html('<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>Zmiany zostały zapisane');

            $('#myModal').append(alert);

            $("#alert").fadeTo(1500, 500).slideUp(500, function(){
                $("#alert").alert('close');
            });
        }
        else if (data.r == 666) {
            var alert = $('<div id="alert" class="alert alert-danger alert-dismissible" style="position: fixed; top: 1%; left: 5%; width: 90%;">');
            alert.html('<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>'+
                'Nie jesteś tutaj mile widziany. Zaloguj się.'
            );

            $('#myModal').append(alert);

            $("#alert").fadeTo(2500, 500).slideUp(500, function(){
                $("#alert").alert('close');
            });
        }
        else {
            $(id+" input[name=force]").attr('value', 1);

            $(id + " td:last button").attr('class', 'btn btn-danger');

            var alert = $('<div id="alert" class="alert alert-danger alert-dismissible" style="position: fixed; top: 1%; left: 5%; width: 90%;">');
            alert.html('<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>'+
                'Ten wiersz został zmodyfikowany przez ' + data.user + ' o godzinie ' + data.time + '. ' +
                'Jeśli chcesz nadpisać modyfikację, ponownie kliknij Zapisz'
            );

            $('#myModal').append(alert);

            $("#alert").fadeTo(2500, 500).slideUp(500, function(){
                $("#alert").alert('close');
            });
        }

    }, "json");
}