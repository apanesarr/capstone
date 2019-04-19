from Location import Location

def test_round_to_region_one():
	loc = Location([])

	assert(loc.roundToRegion(4.5, 7.2) == (5.0, 5.0))

def test_round_to_region_two():
	loc = Location([])

	assert(loc.roundToRegion(5, 5) == (5.0, 5.0))

def test_round_to_region_three():
	loc = Location([])

	assert(loc.roundToRegion(-1.0, -50) == (-1.0, -1.0))

def test_round_to_region_three():
	loc = Location([])

	assert(loc.roundToRegion(80.0, 26.8) == (85.0, 25.0))