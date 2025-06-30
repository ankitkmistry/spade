# Numeric constants
public const PI     = 3.141592653589793238462643383279502884 # pi
public const PI_2   = 1.570796326794896619231321691639751442 # pi / 2
public const PI_4   = 0.785398163397448309615660845819875721 # pi / 4
public const E      = 2.718281828459045235360287471352662498
public const TAU    = 6.283185307179586476925286766559005768

public fun abs(value: int) -> int = 0

# Number theoretic functions
public fun comb(n: int, k: int) -> int = 0
public fun perm(n: int, k: int) -> int = 0
public fun factorial(n: int) -> int = 0
public fun gcd(*numbers: int) -> int = 0

# Floating point arithmetic
public fun abs(value: float) -> float = 0
public fun max(*values: float) -> float = 0
public fun min(*values: float) -> float = 0
public fun ceil(value: float) -> float = 0
public fun floor(value: float) -> float = 0
public fun fmod(x: float, y: float) -> float = 0
public fun fma(x: float, y: float, z: float) -> float = 0
public fun signum(x: float) -> float = 0

# public fun modf(x: float) -> (float, float) = 0 # TODO: after implementing tuples

public fun trunc(x: float) -> int = 0

# Floating point manipulation
public fun copysign(x: float, y: float) -> float = 0
public fun ldexp(x: float, y: int) -> float = false

# public fun frexp(x: float) -> (float, int) = 0 # TODO: after implementing tuples

public fun isclose(a: float, b: float, /, rel_tol=1e-09, abs_tol=0.0) -> bool = false
public fun isfinite(x: float) -> bool = false
public fun isinf(x: float) -> bool = false
public fun isnan(x: float) -> bool = false

# Power, exponential and logarithmic functions
public fun sqrt(x: float) -> float = 0
public fun cbrt(x: float) -> float = 0

public fun exp(x: float) -> float = 0 # e^x
public fun expm1(x: float) -> float = 0 # e^x - 1 (with precision)
public fun exp2(x: float) -> float = 0 # 2^x

public fun log(x: float) -> float = 0                   # ln(x)
public fun log(x: float, base: float) -> float = 0      # log(x) with base
public fun log1p(x: float) -> float = 0                 # ln(1 + x)
public fun log2(x: float) -> float = 0                  # log(x) with base 2
public fun log10(x: float) -> float = 0                 # log(x) with base 10
public fun pow(x: float, y: float) -> float = 0         # x ** y

# Angular conversion
public fun radians(x: float) -> float = 0
public fun degrees(x: float) -> float = 0

# Trigonometric functions
public fun sin(x: float) -> float = 0
public fun cos(x: float) -> float = 0
public fun tan(x: float) -> float = 0

public fun asin(x: float) -> float = 0
public fun acos(x: float) -> float = 0
public fun atan(x: float) -> float = 0
public fun atan2(y: float, x: float) -> float = 0

# Hyperbolic functions
public fun sinh(x: float) -> float = 0
public fun cosh(x: float) -> float = 0
public fun tanh(x: float) -> float = 0

public fun asinh(x: float) -> float = 0
public fun acosh(x: float) -> float = 0
public fun atanh(x: float) -> float = 0

# Special functions
public fun erf(x: float) -> float = 0
public fun erfc(x: float) -> float = 0
public fun gamma(x: float) -> float = 0
public fun lgamma(x: float) -> float = 0
