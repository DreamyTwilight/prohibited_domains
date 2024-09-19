#include <algorithm>
#include <iostream>
#include <string>
#include <set> // заголовок мжно убрать, он не используется
#include <sstream>
#include <string_view> // заголовок можно убрать, он не используется
#include <vector>
#include <map>
#include <fstream>
#include <cassert>

using namespace std;

class Domain {
public:
    Domain() = default;

    Domain(std::string domain) : domain_(std::move(domain)) {
        domain_reverse_ = std::move(ReverseDomainName(domain_));
        count_symbols_ = domain_.size();
    }

    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
}
    std::string GetNameDomain() const {
        return domain_;
    }
    std::string GetReverseNameDomain() const {
        return domain_reverse_;
    }

    bool IsSubdomain(const Domain& other) const {
        assert(this->domain_ != other.domain_); // отличная проверка, но можно заменить assert на обычный if... true (домен является своим поддоменом)
        if (count_symbols_ <= other.count_symbols_) {
            return false;
        }
        return domain_reverse_.find(other.GetReverseNameDomain() + "."s) == 0;
    }

private:
    std::string domain_;
    std::string domain_reverse_;
    size_t count_symbols_ = 0;

    std::string ReverseDomainName(const std::string& domain_) { // стоит рассмотреть реализацию из стандартной библиотеки: std::reverse
        auto position_dot = domain_.find('.');
        size_t first_symbol = 0;
        std::vector<string> catting_domain_name;
        while (position_dot != domain_.npos) {
            catting_domain_name.push_back(domain_.substr(first_symbol, position_dot - first_symbol));
            first_symbol = position_dot + 1;
            position_dot = domain_.find('.', first_symbol);
        }
        catting_domain_name.push_back(domain_.substr(first_symbol, position_dot - first_symbol));

        std::string reverse_name_domain;
        bool first = true;
        for (auto it = catting_domain_name.rbegin(); it < catting_domain_name.rend(); ++it) {
            if (!first) {
                reverse_name_domain += '.';
            }
            first = false;
            reverse_name_domain += *it;
        }
        return reverse_name_domain;
    }
};

class DomainChecker {
public:
    // êîíñòðóêòîð äîëæåí ïðèíèìàòü ñïèñîê çàïðåù¸ííûõ äîìåíîâ ÷åðåç ïàðó èòåðàòîðîâ
    template <typename Itr>
    DomainChecker(const Itr& begin, const Itr& end){
        if (begin != end) {
            std::vector<Domain> forbidden_domains(begin, end);

            std::sort(forbidden_domains.begin(), forbidden_domains.end(), 
                [](const auto l, const auto r) {return l.GetReverseNameDomain() < r.GetReverseNameDomain(); });
            
            Domain curent_domain = forbidden_domains[0];
            forbidden_domains_.emplace(curent_domain.GetReverseNameDomain(), curent_domain);

            for (size_t i = 1; i < forbidden_domains.size(); ++i) {
                if (!(curent_domain == forbidden_domains[i]) && !(forbidden_domains[i].IsSubdomain(curent_domain))) {
                    forbidden_domains_.emplace(forbidden_domains[i].GetReverseNameDomain(), forbidden_domains[i]);
                    curent_domain = forbidden_domains[i];
                }
            }
        }
    }

    // ðàçðàáîòàéòå ìåòîä IsForbidden, âîçâðàùàþùèé true, åñëè äîìåí çàïðåù¸í
    bool IsForbidden(const Domain& domain) const {
        if (forbidden_domains_.empty()) {
            return false;
        }
        std::string name_domain = domain.GetReverseNameDomain();
        if (forbidden_domains_.find(name_domain) != forbidden_domains_.end()) {
            return true;
        }
        auto position_dot = name_domain.find('.');
        while (position_dot != name_domain.npos) {
            if (forbidden_domains_.find(name_domain.substr(0, position_dot)) != forbidden_domains_.end()) {
                return true;
            }
            position_dot = name_domain.find('.', position_dot + 1);
        }
        return false;
    }

private:
    std::map < std::string, Domain > forbidden_domains_;
    size_t max_length_name_domain_ = 0;
};

// ðàçðàáîòàéòå ôóíêöèþ ReadDomains, ÷èòàþùóþ çàäàííîå êîëè÷åñòâî äîìåíîâ èç ñòàíäàðòíîãî âõîäà
template <typename Number>
std::vector<Domain> ReadDomains(std::istream& in, Number num) {
    std::vector<Domain> domains;
    domains.reserve(num);
    std::string name_domain;
    for (size_t i = 0; i < num; ++i) {
        std::getline(in, name_domain);
        domains.push_back(Domain(std::move(name_domain)));
    }
    return domains;
}

template<typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);
    Number num;
    std::istringstream(line) >> num;
    return num;
}

void FuncMain(std::istream& in) {
    const std::vector<Domain> forbidden_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}

int main() {

    std::ifstream in("tst_begin.txt"s);
    assert(in.is_open());

    FuncMain(in);
    in.close();
}
