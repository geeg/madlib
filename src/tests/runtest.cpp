#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "UnitTests-@PORT_ID@"
#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>

struct OutputXML
{
    OutputXML()
    {
        std::string logFileName(boost::unit_test::framework::master_test_suite().p_name);
        logFileName.append(".xml");
        logFile.open(logFileName.c_str());
        assert(logFile.is_open());
        boost::unit_test::unit_test_log.set_stream(logFile);
    }
    ~OutputXML()
    {
        logFile << "</TestLog>" << std::flush;
        logFile.close();
	boost::unit_test::unit_test_log.set_stream(std::cout);
    }
    std::ofstream logFile;
};

BOOST_GLOBAL_FIXTURE(OutputXML)
