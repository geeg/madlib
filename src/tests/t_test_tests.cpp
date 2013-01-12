#include <boost/test/unit_test.hpp>
#include <dbconnector/dbconnector.hpp>
//#include <gtest/gtest.h>
//#include <gmock/gmock.h>
#include "modules/stats/t_test.hpp"
#include "t_test_tests.hpp"

namespace madlib {
namespace modules{
namespace stats{

/* The below code integrates Google Mock*/
/*class BoostTestAdapter: public EmptyTestEventListener {
 
    virtual void OnTestStart(const TestInfo& {
    }
 
    virtual void OnTestPartResult(
            const TestPartResult& testPartResult) {
        if (testPartResult.failed()) {
            stringstream s;
            s << "Mock test failed (file = '"
              << testPartResult.file_name()
              << "', line = "
              << testPartResult.line_number()
              << "): "
              << testPartResult.summary();
            BOOST_FAIL(s.str());
        }
    }
 
    virtual void OnTestEnd(
            const ::testing::TestInfo&) {
    }
 
};

class TestFixture {
public:
 
    TestFixture() {
 
        InitGoogleMock(
                &boost::unit_test::framework::master_test_suite().argc,
                boost::unit_test::framework::master_test_suite().argv);
        TestEventListeners &listeners =
                UnitTest::GetInstance()->listeners();
        // this removes the default error printer
        delete listeners.Release(
                listeners.default_result_printer());
        listeners.Append(new BoostTestAdapter);
 
    }
 
    ~TestFixture() {
        // nothing to tear down
    }
 
};
BOOST_GLOBAL_FIXTURE(TestFixture);
*/

/* Fixture to represent AnyType input block */
struct AnyTypeFixture {
    AnyType input;
    AnyTypeFixture() { 
		double dummy = 1;
		input << dummy << dummy << dummy << dummy << dummy;
	}
     ~ AnyTypeFixture() { }
};

BOOST_FIXTURE_TEST_CASE(test_ucsos, AnyTypeFixture) {
    double x = 0;
    double numX = input[0].getAs<double>();
    double x_sum = input[1].getAs<double>();
    double correctedX_square_sum = input[2].getAs<double>();

    updateCorrectedSumOfSquares(
        numX,x_sum,correctedX_square_sum,
        1, x, 0);
	BOOST_CHECK(numX == 2);
	BOOST_CHECK(x_sum == 1);
	BOOST_CHECK(correctedX_square_sum == 1.5);
    }
}
}
}
