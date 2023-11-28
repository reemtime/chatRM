#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>


namespace boost
{
namespace serialization
{
    class Message
    {
        friend class boost::serialization::access;
        public:
        int status;
        std::string command;
        std::vector<std::string> data;

        template <typename Archive>
        void save(Archive& ar, const unsigned int version) const
        {
            if(version > 0)
            {
                ar & status;
                ar & command;
                ar & data;
            }
        }

        template <typename Archive>
        void load(Archive& ar, const unsigned int version)
        {
            if(version > 0)
            {
                ar & status;
                ar & command;
                ar & data;
            }
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };

    inline std::string serialize_message(Message& message)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive serialized_message(oss);
        serialized_message << message;

        return oss.str();
    }

    inline Message deserialize_message(const std::string& serialized_message)
    {
        Message deserialized_message;
        std::istringstream iss(serialized_message);
        boost::archive::text_iarchive a_msg(iss);
        a_msg >> deserialized_message;
        return deserialized_message;
    }
}
}
BOOST_CLASS_VERSION(boost::serialization::Message, 1)


#endif
