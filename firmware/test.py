import math

import pyserial
import pytest


class Test:
    def __init__(self):
        self.aa = "aa"

    def test(self):
        self.aa = "b"

    def test1(self):
        self.aa = "b"


def test_my_test():
    test = Test()
    test.test()
    assert test.aa == "aa"
