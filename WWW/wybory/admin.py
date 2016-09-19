from django.contrib import admin
from django import forms

from .models import *


class GminaForm(forms.ModelForm):
    def clean(self):
        cleaned_data = super(GminaForm, self).clean()
        mieszkancow = cleaned_data.get("l_mieszkancow")
        uprawnionych = cleaned_data.get("l_uprawnionych")
        wydanych = cleaned_data.get("l_wydanych_kart")
        oddanych = cleaned_data.get("l_oddanych_glosow")
        waznych = cleaned_data.get("l_waznych_glosow")
        glosow1 = cleaned_data.get("l_glosow_1")
        glosow2 = cleaned_data.get("l_glosow_2")

        if mieszkancow < uprawnionych:
            raise forms.ValidationError(
                "Liczba mieszkańców jest mniejsza od liczby uprawnionych"
            )
        if uprawnionych < wydanych:
            raise forms.ValidationError(
                "Liczba uprawnionych jest mniejsza od liczby wydanych kart"
            )
        if wydanych < oddanych:
            raise forms.ValidationError(
                "Liczba wydanych kart jest mniejsza od liczby oddanych głosów"
            )
        if oddanych < waznych:
            raise forms.ValidationError(
                "Liczba oddanych głosów jest mniejsza od liczby ważnych głosów"
            )
        if waznych < glosow1:
            raise forms.ValidationError(
                "Liczba ważnych głosów jest mniejsza od liczby głosów oddanych na pierwszego kandydata"
            )
        if waznych < glosow2:
            raise forms.ValidationError(
                "Liczba ważnych głosów jest mniejsza od liczby głosów oddanych na drugiego kandydata"
            )
        if glosow1 + glosow2 > waznych:
            raise forms.ValidationError(
                "Liczba ważnych głosów jest mniejsza od sumy głosów oddanych na obu kandydatów"
            )

        return self.cleaned_data


class GminaAdmin(admin.ModelAdmin):
    form = GminaForm
    list_display = ('nazwa', 'wojewodztwo')


class KandydatAdmin(admin.ModelAdmin):
    list_display = ('id', 'nazwisko', 'imie')


class WojewodztwoAdmin(admin.ModelAdmin):
    list_display = ('idw', 'nazwa')


admin.site.register(Gmina, GminaAdmin)
admin.site.register(Kandydat, KandydatAdmin)
admin.site.register(Wojewodztwo, WojewodztwoAdmin)