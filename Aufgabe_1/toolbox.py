from typing import List
import math
Vector = List[float]

def mean(xs: List[float]) -> float:
    return sum(xs) / len(xs)


def de_mean(xs: List[float]) -> List[float]:
    """verschiebe xs durch Abziehen des Mittels werts (Ergebnis hat Mittelwert 0)"""
    x_bar = mean(xs)
    return [x - x_bar for x in xs]


def dot(v: Vector, w: Vector) -> float:
    """Berechnet v_1 * w_1 + ... + v_n * w_n"""
    assert len(v) == len(w), "vectors must be same length"

    return sum(v_i * w_i for v_i, w_i in zip(v, w))


def sum_of_squares(v: Vector) -> Vector:
    """Liefert v_1 * v_1 + ... + v_n * v_n"""
    return dot(v, v)


def variance(xs: List[float]) -> float:
    """Ungefähr die duchschnittliche quadrierte Abweichung vom Mittelwert"""
    assert len(xs) >= 2, "variance requires at least two elements"

    n = len(xs)
    deviation = de_mean(xs)
    return sum_of_squares(deviation) / (n - 1)


def standard_deviation(xs: List[float]) -> float:
    """Die Standard abweichung ist die Wurzel der Variance"""
    return math.sqrt(variance(xs))


def data_range(xs: List[float]) -> float:
    return max(xs) - min(xs)