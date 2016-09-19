
from django.shortcuts import render
from django.db.models import Sum
from collections import namedtuple
from django.contrib.auth.views import login
from django.http import HttpResponseRedirect, HttpResponse
from django.core import serializers
from django.views.decorators.csrf import ensure_csrf_cookie
from django.db.models import Q
import json
import datetime
from rest_framework import viewsets, generics
from wybory.serializers import *
from django.http import Http404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework.decorators import api_view
from rest_framework.reverse import reverse
from rest_framework import permissions
from rest_framework import status
from pprint import pprint
from rest_framework.renderers import JSONRenderer

from .models import *


KandydatTup = namedtuple('KandydatTup', 'nazwisko, imie, glosow, procent')
MapColor = namedtuple('MapColor', 'text, color1, color2, value1, value2')

# kolory mapy / legendy
map_color = [
    MapColor('brak danych', '#FFFFFF', '#FFFFFF', None, None),
    MapColor('50.00%', '#BEFFA7', '#BEFFA7', 50.000, 50.000),
    MapColor('>50.00%', '#cce3ff', '#ffdb70', 50.001, 52.269),
    MapColor('&ge;52.27%', '#b4d5fd', '#ffcd70', 52.270, 54.539),
    MapColor('&ge;54.54%', '#99c7ff', '#ffc15e', 54.540, 56.809),
    MapColor('&ge;56.81%', '#7db7fe', '#ffb554', 56.810, 59.079),
    MapColor('&ge;59.08%', '#5aa4fe', '#ffa951', 59.080, 61.349),
    MapColor('&ge;61.35%', '#3590ff', '#ff9d39', 61.350, 63.619),
    MapColor('&ge;63.62%', '#0273fd', '#fe9020', 63.620, 65.889),
    MapColor('&ge;65.89%', '#0260d4', '#ff8609', 65.890, 68.159),
    MapColor('&ge;68.16%', '#014aa3', '#e77900', 68.160, 70.429),
    MapColor('&ge;70.43%', '#003575', '#ce6800', 70.430, 100.000),
]

@ensure_csrf_cookie
def index(request):

    # flip
    total1 = Gmina.objects.all().aggregate(foo=Sum('l_glosow_1'))['foo'] or 0
    total2 = Gmina.objects.all().aggregate(foo=Sum('l_glosow_2'))['foo'] or 0
    flip = 1 if total1 < total2 else 0

    # kandydaci
    if flip:
        total1, total2 = total2, total1
    total = total1 + total2
    if total == 0:
        total = 0.1

    k1 = Kandydat.objects.get(id=1)
    k2 = Kandydat.objects.get(id=2)
    if flip:
        k1, k2 = k2, k1

    kandydat = []
    for x, totx in (k1, total1), (k2, total2):
        kandydat.append(KandydatTup(x.nazwisko.upper(), x.imie, totx, round(totx/total, 4) * 100))
    kandydat1 = kandydat[0]
    kandydat2 = kandydat[1]

    # staty dla całego kraju
    statystyka = {
        'liczba_m': 38028391,
        'powierzchnia': '312685 km<sup>2</sup>',
        'zaludnienie': '121 os/km<sup>2</sup>',
        'liczba_u': Gmina.objects.all().aggregate(foo=Sum('l_uprawnionych'))['foo'],
        'liczba_wk': Gmina.objects.all().aggregate(foo=Sum('l_wydanych_kart'))['foo'],
        'liczba_go': Gmina.objects.all().aggregate(foo=Sum('l_oddanych_glosow'))['foo'],
        'liczba_gw': Gmina.objects.all().aggregate(foo=Sum('l_waznych_glosow'))['foo'],
    }



    # województwa + kolory na mapie
    wojewodztwa = Wojewodztwo.objects.annotate(
        l_waznych_glosow=Sum('gmina__l_waznych_glosow') or 0,
        l_glosow_1=Sum('gmina__l_glosow_1') or 0,
        l_glosow_2=Sum('gmina__l_glosow_2') or 0
    )

    for w in wojewodztwa:
        if flip:
            w.l_glosow_1, w.l_glosow_2 = w.l_glosow_2, w.l_glosow_1
        if w.l_waznych_glosow == 0:
            w.proc1, w.proc2 = 0, 0
        else:
            w.proc1 = round(w.l_glosow_1 / w.l_waznych_glosow, 4) * 100
            w.proc2 = round(w.l_glosow_2 / w.l_waznych_glosow, 4) * 100

        if w.l_waznych_glosow == 0:
            w.color = map_color[0].color1
        else:
            top = max(w.proc1, w.proc2)
            i = 0
            for j, x in zip(reversed(range(len(map_color))), reversed(map_color)):
                if top >= x.value1:
                    i = j
                    break
            w.color = map_color[i].color1 if top == w.proc1 else map_color[i].color2

    # statystyki wg rodzaju gminy
    stat1 = []
    for i, (a, b) in enumerate(RODZAJ_GMINY):
        tmp = Gmina.objects.filter(rodzaj=a).aggregate(
            liczba_wg=Sum('l_waznych_glosow') or 0,
            l1=Sum('l_glosow_1') or 0,
            l2=Sum('l_glosow_2') or 0
        )
        tmp['lp'] = i + 1
        tmp['podzial'] = b
        tmp['rodzaj'] = a
        if flip:
            tmp['l1'], tmp['l2'] = tmp['l2'], tmp['l1']
        if tmp['liczba_wg'] == 0:
            tmp['proc1'] = tmp['proc2'] = 0
        else:
            tmp['proc1'] = round(tmp['l1'] / tmp['liczba_wg'], 4) * 100
            tmp['proc2'] = round(tmp['l2'] / tmp['liczba_wg'], 4) * 100
        stat1.append(tmp)

    # statystyki wg liczby mieszkańców
    bounds = [
        (-1, "x"),
        (5000, "do 5 000"),
        (10000, "od 5 001 do 10 000"),
        (20000, "od 10 001 do 20 000"),
        (50000, "od 20 001 do 50 000"),
        (100000, "od 50 001 do 100 000"),
        (200000, "od 100 001 do 200 000"),
        (500000, "od 200 001 do 500 000"),
        (0, "pow. 500 000")
    ]
    stat2 = [{
        'podzial': 'Statki i zagranica', 'liczba_wg': stat1[2]['liczba_wg'] + stat1[3]['liczba_wg'],
        'l1': stat1[2]['l1'] + stat1[3]['l1'], 'l2': stat1[2]['l2'] + stat1[3]['l2'],
        'lte': -1, 'gte': -1
    }]
    for i in range(1, len(bounds) - 1):
        tmp = Gmina.objects.filter(l_mieszkancow__gte=bounds[i - 1][0]+1, l_mieszkancow__lte=bounds[i][0]).aggregate(
            liczba_wg=Sum('l_waznych_glosow') or 0, l1=Sum('l_glosow_1') or 0, l2=Sum('l_glosow_2') or 0
        )
        tmp['podzial'] = bounds[i][1]
        tmp['gte'] = bounds[i - 1][0]+1
        tmp['lte'] = bounds[i][0]
        stat2.append(tmp)

    tmp = Gmina.objects.filter(l_mieszkancow__gt=bounds[-2][0]).aggregate(
        liczba_wg=Sum('l_waznych_glosow') or 0, l1=Sum('l_glosow_1') or 0, l2=Sum('l_glosow_2') or 0
    )
    tmp['podzial'] = bounds[-1][1]
    tmp['gte'] = bounds[-2][0]-1
    tmp['lte'] = 1000000000000
    stat2.append(tmp)

    if flip:
        for i in range(1, len(stat2)):
            stat2[i]['l1'], stat2[i]['l2'] = stat2[i]['l2'], stat2[i]['l1']
    for x in stat2:
        if x['liczba_wg'] == 0:
            x['proc1'] = x['proc2'] = 0
        else:
            x['proc1'] = round(x['l1'] / x['liczba_wg'], 4) * 100
            x['proc2'] = round(x['l2'] / x['liczba_wg'], 4) * 100

    # render
    return render(request, 'wybory/old_index.html', {
        'flip': flip,
        'kandydat1': kandydat1,
        'kandydat2': kandydat2,
        'mapColor': map_color,
        'statystyka': statystyka,
        'wojewodztwa': wojewodztwa,
        'stat1': stat1,
        'stat2': stat2,
    })


def custom_login(request):
    if request.method == 'GET' and request.user.is_authenticated():
        return HttpResponseRedirect(request.GET.get('next', '/'))
    else:
        return login(request, template_name='wybory/login.html')


def modal(request):
    woj = request.POST.get('woj', '-1')
    typ = request.POST.get('typ', 'x')
    gte = request.POST.get('gte', '-2')
    lte = request.POST.get('lte', '-2')
    data = 0

    if woj != '-1':
        data = Gmina.objects.filter(wojewodztwo__idw=woj)
    elif typ != 'x':
        data = Gmina.objects.filter(rodzaj=typ)
    elif gte != '-2':
        if gte == '-1':
            data = Gmina.objects.filter(Q(rodzaj='s') | Q(rodzaj='z'))
        else:
            data = Gmina.objects.filter(l_mieszkancow__gte=gte, l_mieszkancow__lte=lte)

    ser = serializers.serialize('json', data)
    return HttpResponse(ser, content_type='application/json')


def modal_save(request):
    if request.user.is_authenticated():

        data = request.POST['data']
        force = request.POST['force']

        obj = 0
        for x in serializers.deserialize('json', data):
            obj = x

        db_obj = Gmina.objects.filter(pk=obj.object.pk)[0]

        if force == '0' and str(obj.object.mod_date)[:23] != str(db_obj.mod_date)[:23]:
            error = 1
        else:
            error = 0
            obj.object.mod_date = datetime.datetime.now()
            obj.save()

        if error == 1:
            res = {'r': 1, 'user': 'admin', 'time': str(db_obj.mod_date.strftime('%H:%M:%S'))}
        else:
            res = {'r': 0, 'user': '', 'time': 0}

        ser = json.dumps(res)
        return HttpResponse(ser, content_type='application/json')

    else:
        res = {'r': 666, 'user': '', 'time': 0}
        ser = json.dumps(res)
        return HttpResponse(ser, content_type='application/json')


# REST


class GminaList(generics.ListAPIView):
    queryset = Gmina.objects.all()
    serializer_class = GminaSerializer


class GminaDetail(APIView):
    permission_classes = (permissions.IsAuthenticatedOrReadOnly,)

    def get_object(self, pk):
        try:
            return Gmina.objects.get(pk=pk)
        except Gmina.DoesNotExist:
            raise Http404

    def get(self, request, pk, format=None):
        gmina = self.get_object(pk)
        serializer = GminaSerializer(gmina)
        return Response(serializer.data)

    def post(self, request, pk, format=None):

        g = self.get_object(pk)

        serializer = GminaUpdateSerializer(data=request.data)
        if not serializer.is_valid():
            return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
        gu = serializer.create(serializer.validated_data)

        pprint(request.data)
        pprint(str(g.mod_date)[:23])
        pprint(str(gu.mod_date))

        if not gu.force and str(g.mod_date)[:23] != gu.mod_date:
            gu.conflict = True
            gu.force = True
            gu.author = 'admin'
            gu.mod_date = str(g.mod_date)[:23]
        else:
            g.l_mieszkancow = gu.l_mieszkancow
            g.l_uprawnionych = gu.l_uprawnionych
            g.l_wydanych_kart = gu.l_wydanych_kart
            g.l_oddanych_glosow = gu.l_oddanych_glosow
            g.l_waznych_glosow = gu.l_waznych_glosow
            g.l_glosow_1 = gu.l_glosow_1
            g.l_glosow_2 = gu.l_glosow_2
            g.save()

            pprint(vars(g))
            print("\n\n")
            pprint(vars(gu))
            print("\n\n")
            pprint(vars(Gmina.objects.get(pk=pk)))

            gu.mod_date = g.mod_date
            gu.conflict = False

        ser = GminaUpdateSerializer(gu)

        return Response(ser.data, status=status.HTTP_200_OK)


class GenericList(APIView):
    serializer = GminaSerializer

    def get_object(self):
        return Gmina.objects.all()

    def get(self, request, format=None):
        objects = self.get_object()
        serializer = self.serializer(objects, many=True)
        return Response(serializer.data)


class GenericDetail(APIView):

    def get_object(self, pk):
        return Gmina.objects.all()

    def get(self, request, pk, format=None):
        gminy = self.get_object(pk)
        serializer = GminaSerializer(gminy, many=True)
        return Response(serializer.data)


class WojewodztwoGminaList(GenericList):
    serializer = WojewodztwoGminaSerializer

    def get_object(self):
        list = []
        for x in Wojewodztwo.objects.all():
            list.append(WojewodztwoGmina(x))
        return list


class WojewodztwoGminaDetail(GenericDetail):

    def get_object(self, pk):
        try:
            return Gmina.objects.all().filter(wojewodztwo=Wojewodztwo.objects.get(idw=pk))
        except Wojewodztwo.DoesNotExist:
            raise Http404


class GminaRodzajList(GenericList):
    serializer = GminaRodzajSerializer

    def get_object(self):
        list = []
        for x in RODZAJ_GMINY:
            list.append(GminaRodzaj(x[0]))
        return list


class GminaRodzajDetail(GenericDetail):

    def get_object(self, pk):
        return Gmina.objects.all().filter(rodzaj=pk)


class GminaRozmiarList(GenericList):
    serializer = GminaRozmiarSerializer

    def get_object(self):
        list = []
        for x in ROZMIAR_GMINY.items():
            list.append(GminaRozmiar(x[1]))
        return list


class GminaRozmiarDetail(GenericDetail):

    def get_object(self, pk):
        if pk == '1':
            gminy = Gmina.objects.all().filter(Q(nazwa='Statki') | Q(nazwa='Zagranica'))
        elif pk == '9':
            gminy = Gmina.objects.filter(l_mieszkancow__gte=ROZMIAR_GMINY[pk][2])
        else:
            rozmiar = ROZMIAR_GMINY[pk]
            gminy = Gmina.objects.filter(l_mieszkancow__gte=rozmiar[2], l_mieszkancow__lte=rozmiar[3])
        return gminy


class SummaryView(APIView):

    def get(self, request, format=None):
        s = Summary()
        serializer = SummarySerializer(s)
        return Response(serializer.data)


def user_view(request):
    if request.user.is_authenticated():
        response = '{"auth":true, "user":"' + request.user.username + '"}'
    else:
        response = '{"auth":false}'
    return HttpResponse(response, content_type='application/json')


def map_colors(request):
    return HttpResponse(JSONRenderer().render(map_color), content_type='application/json')
