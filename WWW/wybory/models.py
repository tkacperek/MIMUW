from django.contrib.auth.models import User
from django.db import models
import datetime
from django.db.models import Sum, Q
from collections import OrderedDict
from pprint import pprint


class Kandydat(models.Model):
    nazwisko = models.CharField("Nazwisko", max_length=100, default="")
    imie = models.CharField("Imię", max_length=100, default="")

    def __str__(self):
        return self.nazwisko + " " + self.imie

    def __unicode__(self):
        return self.__str__()

    class Meta:
        verbose_name = "Kandydat"
        verbose_name_plural = "Kandydaci"


class Wojewodztwo(models.Model):
    idw = models.IntegerField("Id", default=0)
    nazwa = models.CharField(max_length=100, default="")

    def __str__(self):
        return self.nazwa

    def __unicode__(self):
        return self.__str__()

    class Meta:
        verbose_name = "Województwo"
        verbose_name_plural = "Województwa"


RODZAJ_GMINY = (
    ('m', 'Miasto'),
    ('w', 'Wieś'),
    ('s', 'Statki'),
    ('z', 'Zagranica'),
)

RODZAJ_GMINY_dict = {
    'm': ('m', 'Miasto'),
    'w': ('w', 'Wieś'),
    's': ('s', 'Statki'),
    'z': ('z', 'Zagranica'),
}


class Gmina(models.Model):
    nazwa = models.CharField("Nazwa", max_length=100, default="")
    wojewodztwo = models.ForeignKey(Wojewodztwo, on_delete=models.CASCADE)
    rodzaj = models.CharField("Rodzaj", max_length=1, choices=RODZAJ_GMINY, default='m')
    l_mieszkancow = models.IntegerField("Liczba mieszkańców", default=0)
    l_uprawnionych = models.IntegerField("Liczba uprawnionych", default=0)
    l_wydanych_kart = models.IntegerField("Liczba wydanych kart", default=0)
    l_oddanych_glosow = models.IntegerField("Liczba oddanych głosów", default=0)
    l_waznych_glosow = models.IntegerField("Liczba ważnych głosów", default=0)
    l_glosow_1 = models.IntegerField("Liczba głosów na kandydata 1", default=0)
    l_glosow_2 = models.IntegerField("Liczba głosów na kandydata 2", default=0)
    mod_date = models.DateTimeField("Ostatnia modyfikacja", auto_now=True)

    def __str__(self):
        return self.nazwa

    def __unicode__(self):
        return self.__str__()

    class Meta:
        verbose_name = "Gmina"
        verbose_name_plural = "Gminy"
        unique_together = (("nazwa", "wojewodztwo"),)


class CombinedModel(models.Model):
    l_mieszkancow = models.IntegerField("Liczba mieszkańców", default=0)
    l_uprawnionych = models.IntegerField("Liczba uprawnionych", default=0)
    l_wydanych_kart = models.IntegerField("Liczba wydanych kart", default=0)
    l_oddanych_glosow = models.IntegerField("Liczba oddanych głosów", default=0)
    l_waznych_glosow = models.IntegerField("Liczba ważnych głosów", default=0)
    l_glosow_1 = models.IntegerField("Liczba głosów na kandydata 1", default=0)
    l_glosow_2 = models.IntegerField("Liczba głosów na kandydata 2", default=0)

    def agreg(self, queryset):
        gminy = queryset.aggregate(
            l_mieszkancow=Sum('l_mieszkancow'),
            l_uprawnionych=Sum('l_uprawnionych'),
            l_wydanych_kart=Sum('l_wydanych_kart'),
            l_oddanych_glosow=Sum('l_oddanych_glosow'),
            l_waznych_glosow=Sum('l_waznych_glosow'),
            l_glosow_1=Sum('l_glosow_1'),
            l_glosow_2=Sum('l_glosow_2')
        )
        self.l_mieszkancow = gminy['l_mieszkancow']
        self.l_uprawnionych = gminy['l_uprawnionych']
        self.l_wydanych_kart = gminy['l_wydanych_kart']
        self.l_oddanych_glosow = gminy['l_oddanych_glosow']
        self.l_waznych_glosow = gminy['l_waznych_glosow']
        self.l_glosow_1 = gminy['l_glosow_1']
        self.l_glosow_2 = gminy['l_glosow_2']


class WojewodztwoGmina(CombinedModel):
    idw = models.IntegerField("Id", default=0)
    nazwa = models.CharField(max_length=100, default="")

    def __init__(self, wojewodztwo):
        self.idw = wojewodztwo.idw
        self.nazwa = wojewodztwo.nazwa
        gminy = Gmina.objects.all().filter(wojewodztwo=wojewodztwo)
        self.agreg(gminy)


class GminaRodzaj(CombinedModel):
    rodzaj = RODZAJ_GMINY[0]

    def __init__(self, rodzaj):
        self.rodzaj = RODZAJ_GMINY_dict[rodzaj]
        gminy = Gmina.objects.all().filter(rodzaj=rodzaj)
        self.agreg(gminy)


ROZMIAR_GMINY = OrderedDict([
    ('1', ('1', 'Statki i zagranica', -1, -1)),
    ('2', ('2', 'do 5 000', 0, 5000)),
    ('3', ('3', 'od 5 001 do 10 000', 5001, 10000)),
    ('4', ('4', 'od 10 001 do 20 000', 10001, 20000)),
    ('5', ('5', 'od 20 001 do 50 000', 20001, 50000)),
    ('6', ('6', 'od 50 001 do 100 000', 50001, 100000)),
    ('7', ('7', 'od 100 001 do 200 000', 100001, 200000)),
    ('8', ('8', 'od 200 001 do 500 000', 200001, 500000)),
    ('9', ('9', 'pow. 500 000', 500001, -1))
])


class GminaRozmiar(CombinedModel):
    rozmiar = ROZMIAR_GMINY['1']

    def __init__(self, rozmiar):
        self.rozmiar = rozmiar
        if rozmiar[0] == '1':
            gminy = Gmina.objects.all().filter(Q(nazwa='Statki') | Q(nazwa='Zagranica'))
        elif rozmiar[0] == '9':
            gminy = Gmina.objects.filter(l_mieszkancow__gte=rozmiar[2])
        else:
            gminy = Gmina.objects.filter(l_mieszkancow__gte=rozmiar[2], l_mieszkancow__lte=rozmiar[3])
        self.agreg(gminy)


class Summary(CombinedModel):
    powierzchnia = '312685'
    zaludnienie = '121'
    kandydat1 = Kandydat.objects.get(id=1)
    kandydat2 = Kandydat.objects.get(id=2)
    flip = models.BooleanField(default=False)

    def __init__(self):
        gminy = Gmina.objects.all()
        self.agreg(gminy)
        self.l_mieszkancow = 38028391
        self.flip = self.l_glosow_1 < self.l_glosow_2


class GminaUpdate(CombinedModel):
    force = models.BooleanField()
    conflict = models.BooleanField(default=False)
    author = models.CharField(max_length=100, default="")
    mod_date = models.CharField(max_length=100, default="")

    def __init__(self, gmina=None, dict=None):
        if not gmina is None:
            self.l_mieszkancow = gmina.l_mieszkancow
            self.l_uprawnionych = gmina.l_uprawnionych
            self.l_wydanych_kart = gmina.l_wydanych_kart
            self.l_oddanych_glosow = gmina.l_oddanych_glosow
            self.l_waznych_glosow = gmina.l_waznych_glosow
            self.l_glosow_1 = gmina.l_glosow_1
            self.l_glosow_2 = gmina.l_glosow_2
            self.mod_date = str(gmina.mod_date)[:23]

            self.force = False
            self.conflict = False
            self.author = ''
        else:
            self.l_mieszkancow = dict.get('l_mieszkancow')
            self.l_uprawnionych = dict.get('l_uprawnionych')
            self.l_wydanych_kart = dict.get('l_wydanych_kart')
            self.l_oddanych_glosow = dict.get('l_oddanych_glosow')
            self.l_waznych_glosow = dict.get('l_waznych_glosow')
            self.l_glosow_1 = dict.get('l_glosow_1')
            self.l_glosow_2 = dict.get('l_glosow_2')

            self.force = dict.get('force')
            self.conflict = dict.get('conflict')
            self.author = dict.get('author')
            self.mod_date = str(dict.get('mod_date'))[:23]
