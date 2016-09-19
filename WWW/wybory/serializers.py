
from rest_framework import serializers as rest_serializers
from wybory.models import *
from pprint import pprint


class GminaSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = Gmina
        fields = (
            'pk',
            'nazwa',
            'wojewodztwo',
            'rodzaj',
            'l_mieszkancow',
            'l_uprawnionych',
            'l_wydanych_kart',
            'l_oddanych_glosow',
            'l_waznych_glosow',
            'l_glosow_1',
            'l_glosow_2',
            'mod_date'
        )


class WojewodztwoSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = Wojewodztwo
        fields = (
            'idw',
            'nazwa'
        )


class WojewodztwoGminaSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = WojewodztwoGmina
        fields = (
            'idw',
            'nazwa',
            'l_mieszkancow',
            'l_uprawnionych',
            'l_wydanych_kart',
            'l_oddanych_glosow',
            'l_waznych_glosow',
            'l_glosow_1',
            'l_glosow_2'
        )


class GminaRodzajSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = GminaRodzaj
        fields = (
            'rodzaj',
            'l_mieszkancow',
            'l_uprawnionych',
            'l_wydanych_kart',
            'l_oddanych_glosow',
            'l_waznych_glosow',
            'l_glosow_1',
            'l_glosow_2'
        )


class GminaRozmiarSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = GminaRozmiar
        fields = (
            'rozmiar',
            'l_mieszkancow',
            'l_uprawnionych',
            'l_wydanych_kart',
            'l_oddanych_glosow',
            'l_waznych_glosow',
            'l_glosow_1',
            'l_glosow_2'
        )


class KandydatSerializer(rest_serializers.ModelSerializer):
    class Meta:
        model = Kandydat
        fields = (
            'nazwisko',
            'imie'
        )


class SummarySerializer(rest_serializers.Serializer):
    powierzchnia = rest_serializers.CharField()
    zaludnienie = rest_serializers.CharField()
    kandydat1 = KandydatSerializer()
    kandydat2 = KandydatSerializer()
    l_mieszkancow = rest_serializers.IntegerField()
    l_uprawnionych = rest_serializers.IntegerField()
    l_wydanych_kart = rest_serializers.IntegerField()
    l_oddanych_glosow = rest_serializers.IntegerField()
    l_waznych_glosow = rest_serializers.IntegerField()
    l_glosow_1 = rest_serializers.IntegerField()
    l_glosow_2 = rest_serializers.IntegerField()
    flip = rest_serializers.BooleanField()


class GminaUpdateSerializer(rest_serializers.Serializer):
    l_mieszkancow = rest_serializers.IntegerField()
    l_uprawnionych = rest_serializers.IntegerField()
    l_wydanych_kart = rest_serializers.IntegerField()
    l_oddanych_glosow = rest_serializers.IntegerField()
    l_waznych_glosow = rest_serializers.IntegerField()
    l_glosow_1 = rest_serializers.IntegerField()
    l_glosow_2 = rest_serializers.IntegerField()

    force = rest_serializers.BooleanField(default=False)
    conflict = rest_serializers.BooleanField(default=False)
    author = rest_serializers.CharField(allow_blank=True)
    mod_date = rest_serializers.CharField(allow_blank=True)

    def create(self, validated_data):
        return GminaUpdate(dict=validated_data)
