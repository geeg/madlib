#include <boost/test/unit_test.hpp>

 int add(int i, int j){
 	return i + j;
 }         

BOOST_AUTO_TEST_CASE(test_dummy) {
	BOOST_CHECK(add(2,2) == 4);
}
