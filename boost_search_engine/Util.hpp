#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
#include "log.hpp"

namespace NS_util
{
    class FileUtil
    {
    public:
        static bool ReadFile(const std::string &file_path, std::string *out)
        {
            std::ifstream in(file_path, std::ios::in);
            if (!in.is_open())
            {
                std::cerr << "open file" << file_path << "error" << std::endl;
                return false;
            }

            std::string line;
            while (std::getline(in, line))
            {
                *out += line;
            }

            in.close();
            return true;
        }
    };

    class StringUtil
    {
    public:
        static void Split(const std::string &target, std::vector<std::string> *out, std::string sep)
        {

            boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on);
        }
    };

    const char *const DICT_PATH = "./dict/jieba.dict.utf8";
    const char *const HMM_PATH = "./dict/hmm_model.utf8";
    const char *const USER_DICT_PATH = "./dict/user.dict.utf8";
    const char *const IDF_PATH = "./dict/idf.utf8";
    const char *const STOP_WORD_PATH = "./dict/stop_words.utf8";

    class JiebaUtil
    {
    private:
        cppjieba::Jieba jieba;
        std::unordered_map<std::string, bool> stop_words;

    private:
        JiebaUtil() : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH) {}
        JiebaUtil(const JiebaUtil &) = delete;

        static JiebaUtil *instance;

    public:
        static JiebaUtil *get_instance()
        {
            static std::mutex mtx;
            if (instance == nullptr)
            {
                mtx.lock();
                if (instance == nullptr)
                {
                    instance = new JiebaUtil();
                    instance->InitJiebaUtil();
                }
                mtx.unlock();
            }

            return instance;
        }

        void InitJiebaUtil()
        {
            std::ifstream in(STOP_WORD_PATH);
            if (!in.is_open())
            {
                LOG(FATAL, "???????????????????????????");
                return;
            }

            std::string line;
            while (std::getline(in, line))
            {
                stop_words.insert({line, true});
            }

            in.close();
        }

        void CutStringHelper(const std::string &src, std::vector<std::string> *out)
        {
            jieba.CutForSearch(src, *out);
            for (auto iter = out->begin(); iter != out->end();)
            {
                auto stop_word = stop_words.find(*iter);
                if (stop_word != stop_words.end())
                {
                    out->erase(iter);
                }
                else
                {
                    iter++;
                }
            }
        }

    public:
        static void CutString(const std::string &src, std::vector<std::string> *out)
        {
            NS_util::JiebaUtil::get_instance()->CutStringHelper(src, out);
        }
    };
    JiebaUtil *JiebaUtil::instance = nullptr;
}
