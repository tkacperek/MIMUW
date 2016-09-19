
import time

from django.contrib.staticfiles.testing import StaticLiveServerTestCase
from selenium.webdriver.firefox.webdriver import WebDriver
from django.test import TestCase, Client
from django.core import serializers

from .models import Gmina


class ModalViewTest(TestCase):
    fixtures = ['wybory/init.yaml']
    client = 0

    def setUp(self):
        self.client = Client()
        response = self.client.post('/login/', {'username': 'admin', 'password': 'haslo123'})
        self.assertEqual(response.status_code, 302)

    def test_whatever(self):
        response = self.client.post('/ajax/', { 'woj': '0', 'typ': 'x', 'gte': '-2', 'lte': '-2'})
        self.assertEqual(response.status_code, 200)
        for x in serializers.deserialize('json', response.content):
            obj = x
        self.assertEqual(obj.object.pk, 8)


class MySeleniumTests(StaticLiveServerTestCase):
    fixtures = ['wybory/init.yaml']

    @classmethod
    def setUpClass(cls):
        super(MySeleniumTests, cls).setUpClass()
        cls.selenium1 = WebDriver()
        cls.selenium2 = WebDriver()
        cls.selenium1.set_window_position(0, 0)
        cls.selenium1.set_window_size(1200, 400)
        cls.selenium2.set_window_position(0, 400)
        cls.selenium2.set_window_size(1200, 500)

    @classmethod
    def tearDownClass(cls):
        cls.selenium1.quit()
        cls.selenium2.quit()
        super(MySeleniumTests, cls).tearDownClass()

    def test_login(self):

        gm = Gmina.objects.filter(pk=7)[0]
        self.assertEqual(gm.l_glosow_1, 1281)
        self.assertEqual(gm.l_glosow_2, 1070)

        for selenium in [self.selenium1, self.selenium2]:
            selenium.get('%s%s' % (self.live_server_url, '/login/'))
            username_input = selenium.find_element_by_name("username")
            username_input.send_keys('admin')
            password_input = selenium.find_element_by_name("password")
            password_input.send_keys('haslo123')
            selenium.find_element_by_xpath('//button[@type="submit"]').click()

            selenium.find_element_by_css_selector("#firstTable tr:nth-child(4) a").click()
            time.sleep(1)
            selenium.find_element_by_css_selector("#mtr7 button").click()

        for selenium, val1, val2 in [(self.selenium1, '1280', '1071'), (self.selenium2, '1279', '1072')]:
            in1 = selenium.find_element_by_css_selector("#mtr7 td[name=l_glosow_1] input")
            in1.clear()
            in1.send_keys(val1)
            in2 = selenium.find_element_by_css_selector("#mtr7 td[name=l_glosow_2] input")
            in2.clear()
            in2.send_keys(val2)

            selenium.find_element_by_css_selector("#mtr7 button").click()

        time.sleep(2)
        self.selenium2.find_element_by_css_selector("#mtr7 button").click()

        gm = Gmina.objects.filter(pk=7)[0]
        self.assertEqual(gm.l_glosow_1, 1279)
        self.assertEqual(gm.l_glosow_2, 1072)

        time.sleep(2)
