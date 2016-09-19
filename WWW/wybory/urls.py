from django.conf.urls import url, include
from django.contrib.auth import views as auth_views
from django.views.generic import TemplateView

from . import views


urlpatterns = [
    url(r'^$', TemplateView.as_view(template_name="wybory/index.html")),
    url(r'^old$', views.index, name='old_index'),
    url('^login/$', views.custom_login),
    url('^logout/$', auth_views.logout, {'template_name': 'wybory/logout.html'}),
    url('^ajax/$', views.modal),
    url('^modal_save/$', views.modal_save),

    url(r'^gminy/$', views.GminaList.as_view()),
    url(r'^gminy/(?P<pk>[0-9]+)/$', views.GminaDetail.as_view()),
    url(r'^wojewodztwa/$', views.WojewodztwoGminaList.as_view()),
    url(r'^wojewodztwa/(?P<pk>[0-9]+)/$', views.WojewodztwoGminaDetail.as_view()),
    url(r'^rodzaje/$', views.GminaRodzajList.as_view()),
    url(r'^rodzaje/(?P<pk>[mwsz])/$', views.GminaRodzajDetail.as_view()),
    url(r'^rozmiary/$', views.GminaRozmiarList.as_view()),
    url(r'^rozmiary/(?P<pk>[1-9])/$', views.GminaRozmiarDetail.as_view()),
    url(r'^summary/$', views.SummaryView.as_view()),
    url(r'^user/$', views.user_view),
    url(r'^map_colors/$', views.map_colors),
]
