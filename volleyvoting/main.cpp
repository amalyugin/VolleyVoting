// best.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <iomanip>


struct voting_data
{
	int tour;
	std::pair<std::string, std::string> teams;
	std::string players[3];
	std::string voter;
};

struct player_info
{
	std::string team;
	char pos = '-'; // -/d/s/l/b/o   не определено/диагональный/св€зующий/либеро/блокирующий/доигровщик
};

struct voter_info
{
	int n = 0;
};

struct player_stat
{
	double sum = 0;
	double av = 0;
	double max = 0;

	int sumPlaces[3] = { 0, 0, 0 };
	int avPlaces[3] = { 0, 0, 0 };
	int maxPlaces[3] = { 0, 0, 0 };

	//double auxSum = 0;
};

typedef std::pair<std::string, player_info> playerpair;
typedef std::pair<std::string, voter_info> voterpair;
typedef std::pair<std::string, player_stat> statpair;



void ltrim(std::string& s) {
	size_t first_non_space = s.find_first_not_of(" \t\n\r\f\v");

	if (std::string::npos == first_non_space) {
		s.clear();
	}
	else {
		s.erase(0, first_non_space);
	}
}

std::vector<std::string> getWords(const std::string& str) {
	std::vector<std::string> words;
	std::stringstream ss(str);
	std::string word;

	while (ss >> word) {
		words.push_back(word);
	}

	return words;
}

bool icompare_pred(unsigned char a, unsigned char b)
{
	return std::tolower(a) == std::tolower(b);
}

bool icompare(std::string const& a, std::string const& b)
{
	if (a.length() == b.length()) {
		return std::equal(b.begin(), b.end(),
			a.begin(), icompare_pred);
	}
	else {
		return false;
	}
}

std::vector<std::string> split(std::string const& str, std::string const& Separator)
{
	std::vector<std::string> answer;
	std::string temp;
	int len = str.size();
	for (int i = 0; i < len; i++)
	{
		bool isSeparator = false;
		for (int j = 0; j < Separator.length(); j++)
		{
			if (str[i] == Separator[j])
				isSeparator = true;
		}
		if (!isSeparator)
		{
			temp += str[i];
			continue;
		}
		if (temp != "")
			answer.push_back(temp);
		temp = "";
	}
	if (temp != "")
		answer.push_back(temp);
	return answer;
}

std::string strtolower(const std::string& str)
{
	std::string s = str;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

std::string findbynamepart(const std::vector<std::string>& c, const std::string& na)
{
	for (auto it = c.begin(); it != c.end(); ++it)
	{
		std::string na1 = strtolower(na);

		if ((*it).find(na1) == 0)//std::string::npos)
			return *it;
	}

	return "";
}

//сравнение по разнице до n символов (опечатка, не хватает); возвращает ко-во опечаток, нехваток
int almostCompare(int n, std::string const& s1, std::string const& s2)
{
	return 0;
}


std::string getToursString(std::vector<int> ts)
{
	std::vector<std::pair<int, int> > ps;
	ps.push_back(std::make_pair(ts[0], ts[0]));
	for (size_t i = 1; i < ts.size(); i++)
	{
		if (ts[i] == ps.back().second + 1) ps.back().second++;
		else ps.push_back(std::make_pair(ts[i], ts[i]));
	}

	std::string str;
	for (int i = 0; i < ps.size(); ++i)
	{
		if (i != 0) str += ", ";
		str += std::to_string(ps[i].first) + "-" + std::to_string(ps[i].second);
	}
	return str;
}

std::string addplayer(std::vector<playerpair>& players, std::string const& p, bool& add)
{
	add = false;
	auto p1 = strtolower(p);
	for (int i = 0; i < players.size(); ++i)
	{
		if (players[i].first.find(p1) != std::string::npos)
			return players[i].first;
	}

	add = true;
	players.push_back(std::make_pair(p1, player_info()));
	return p1;
}



void writeplayer(std::ofstream& f, const statpair& s, int type /*0 - sum, 1 - av, 2 - max*/)
{
	double b = s.second.sum;
	const int* places = s.second.sumPlaces;
	if (type == 1)
	{
		b = s.second.av;
		places = s.second.avPlaces;
	}
	else if (type == 2)
	{
		b = s.second.max;
		places = s.second.maxPlaces;
	}

	f << std::setw(15) << s.first
		<< "  " << std::setw(8) << std::fixed << std::setprecision(2) << b
		<< "  " << std::setw(3) << places[0]
		<< "  " << std::setw(3) << places[1]
		<< "  " << std::setw(3) << places[2]
		<< std::endl;

}


bool comparestat(statpair const& p0, statpair const& p1, int ty)
{
	const double eps = 1e-4;
	double b0 = (ty == 0) ? p0.second.sum : (ty == 1 ? p0.second.av : p0.second.max);
	double b1 = (ty == 0) ? p1.second.sum : (ty == 1 ? p1.second.av : p1.second.max);

	if (b0 >= b1 + eps) return true;
	else if (abs(b0 - b1) < eps)
	{
		const int* places0 = (ty == 0) ? p0.second.sumPlaces : (ty == 1 ? p0.second.avPlaces : p0.second.maxPlaces);
		const int* places1 = (ty == 0) ? p1.second.sumPlaces : (ty == 1 ? p1.second.avPlaces : p1.second.maxPlaces);
		if (places0[0] > places1[0]
			|| places0[0] == places1[0] && places0[1] > places1[1]
			|| places0[0] == places1[0] && places0[1] == places1[1] && places0[2] > places1[2]
			)
			return true;
	}
	return false;
}


int Ntours = 0;
int Npo = 0;
std::vector<int> maxTourInPo;
int playerScores[3] = { 3, 2, 1 };
int playerPoScores[3] = { 3, 2, 1 };


//#define USE_MAX_METHOD

int TourStringToNumber(const std::string& str)
{
	int res = -1;
	bool isPo = (str.find_first_of('/') != std::string::npos);
	if (!isPo)
		res = std::atoi(str.c_str());
	else
	{
		//"1/2_4"
		auto vec = split(str, "/_");

		int pow2 = std::atoi(vec[1].c_str());
		int p = 0;
		while (pow2 != 1)
		{
			pow2 /= 2;
			++p;
		}

		int po = (int)(maxTourInPo.size()) - p - 1;
		int tour = std::atoi(vec[2].c_str()) - 1;
		res = Ntours + 1 + tour;
		for (int i = 0; i < po; ++i)
			res += maxTourInPo[i];
	}
	return res;
}

int main(int argc, char* argv[])
{

	std::string dataFileName(argv[1]);
	std::string resultFileName(argv[2]);

// 	std::string m_strText = "ASDFGHJKLQWERTYUIOPZXCVBNM$:;#јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„Ўў№џЏЁёя";
// 	std::string old = std::setlocale(LC_ALL, nullptr);
// 	std::setlocale(LC_ALL, "rus");
// 	std::transform(m_strText.begin(), m_strText.end(), m_strText.begin(), ::tolower);
// 	std::setlocale(LC_ALL, old.c_str());

	//std::wstring teststr = L"јЅ¬ Ќƒ„—Ћ¬џя";
	//std::transform(teststr.begin(), teststr.end(), teststr.begin(), [](unsigned char c) {return std::tolower(c); });

	std::setlocale(LC_ALL, "Russian");

// 	std::string m_strText = "ASDFGHJKLQWERTYUIOPZXCVBNM$:;#јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„Ўў№џЏЁёя";
// 	std::transform(m_strText.begin(), m_strText.end(), m_strText.begin(), ::tolower);

	std::ifstream wif(dataFileName);
	
	std::vector<std::string> teams;
	std::vector<playerpair> players;

	std::vector<voting_data> votings;

	std::string log;

	std::map<std::string, player_stat> stats;

	std::vector<int> votingTours;

	std::set<std::string> excludeVotMans;
	std::map<std::string, voter_info> includeVotMans;

	int lineCounter = 0;

	if (wif.is_open()) {
 		std::string line;
		// Read the file line by line
		while (std::getline(wif, line)) {

			lineCounter++;

			ltrim(line);

			size_t commentpos = line.find_first_of('#');
			if (commentpos != std::string::npos)
				line.erase(commentpos);

			if (line.empty() || line[0] == '#')
				continue;

			std::vector<std::string> words = getWords(line);

			if (icompare(words[0], "команды:"))
			{
				for (int i = 1; i < words.size(); ++i) 
					teams.push_back(words[i]);
				continue;
			}

			if (words[0].find("туры") == 0)
			{
				Ntours = std::atoi(words[1].c_str());
				continue;
			}

			if (words[0].find("стадии_плэйофф") == 0)
			{
				Npo = std::atoi(words[1].c_str());
				continue;
			}

			if (words[0].find("матчи_плэйофф") == 0)
			{
				for (int i = 0; i < Npo; ++i)
					maxTourInPo.push_back(std::atoi(words[i + 1].c_str()));
				continue;
			}

			if (words[0].find("баллы_регул€рный") == 0)
			{
				for (int i = 0; i < 3; ++i)
					playerScores[i] = std::atoi(words[i + 1].c_str());
				continue;
			}

			if (words[0].find("баллы_плэйофф") == 0)
			{
				for (int i = 0; i < 3; ++i)
					playerPoScores[i] = std::atoi(words[i + 1].c_str());
					continue;
			}

			//TODO: игроки(лен): озерова(св) рубан(до) гарелик(ди) романова(до) мишагина(св) мельникова(бл)
			if (words[0].find("игроки") == 0)
			{
				std::vector<std::string> strs = split(words[0], "():");
				//TODO

				for (int i = 1; i < words.size(); ++i)
					players.push_back(std::make_pair(words[i], player_info()));
				continue;
			}


			if (words[0].find("исключить") == 0)
			{
				for (int i = 1; i < words.size(); ++i)
					excludeVotMans.insert(words[i]);
				continue;
			}

			if (words.size() < 7)
			{
				log += "bad line " + std::to_string(lineCounter);
				continue;
			}
			else if (words.size() > 7)
				log += "very big line " + std::to_string(lineCounter) + "\n";

			voting_data vot;
			vot.tour = TourStringToNumber(words[0]);
			vot.teams.first = findbynamepart(teams, words[1]);
			if (vot.teams.first == "")
				log += "bad 1st team in line " + std::to_string(lineCounter) + "\n";
			vot.teams.second = findbynamepart(teams, words[2]);
			if (vot.teams.second == "") log += "bad 2nd team in line " + std::to_string(lineCounter) + "\n";

			for (int k = 0; k < 3; ++k)
			{
				bool add = false;
				vot.players[k] = addplayer(players, words[k + 3], add);
				if (add) log += "add player " + std::to_string(players.size()) + " = " + vot.players[k] + "  line " + std::to_string(lineCounter) + "\n";
			}

			std::string voter = strtolower(words[6]);
			vot.voter = voter;

			bool addVote = true;

			for (auto& em : excludeVotMans)
			{
				if (em.find(vot.voter) == 0)
				{
					addVote = false;
					break;
				}
			}

			if (addVote)
			{
				auto it = includeVotMans.find(voter);
				if (it != includeVotMans.end())
					it->second.n += 1;
				else
				{
					voter_info vi; vi.n = 1;
					includeVotMans[voter] = vi;
				}

				votings.push_back(vot);
			}
		}
		wif.close();

		int beginTour = TourStringToNumber(std::string(argv[3]));
		int endTour = TourStringToNumber(std::string(argv[4]));

		//подсчет баллов
		if (!votings.empty() && !players.empty() && !teams.empty())
		{
			for (int i = 0; i < players.size(); ++i)
				stats[players[i].first] = player_stat();

			std::sort(votings.begin(), votings.end(), 
				[](const voting_data& a, const voting_data& b) { return a.tour < b.tour; });

			int i0 = 0;
			int i1 = 0;
			while (i0 < votings.size())
			{
				while (i1 < votings.size() && votings[i0].tour == votings[i1].tour) ++i1;

				if (votings[i0].tour >= beginTour && votings[i0].tour <= endTour)
				{
					votingTours.push_back(votings[i0].tour);

					std::sort(votings.begin() + i0, votings.begin() + i1,
						[](const voting_data& a, const voting_data& b) {return a.teams < b.teams; });

					//подсчет
					int j0 = i0;
					int j1 = i0;
					while (j0 < i1)
					{
						while (j1 < i1 && votings[j0].teams == votings[j1].teams) j1++;

						int n = j1 - j0;

						//av, max
						struct pltype 
						{
							double s = 0;
							int places[3] = { 0, 0, 0 };
							int p = -1;
						};

						std::map<std::string, pltype> auxSums;

						int* plsc = (votings[i0].tour <= Ntours) ? playerScores : playerPoScores;

						for (int j = j0; j < j1; ++j)
						{
							for (int k = 0; k < 3; ++k)
							{
								auto ait = auxSums.find(votings[j].players[k]);
								if (ait == auxSums.end())
									ait = auxSums.insert(std::make_pair(votings[j].players[k], pltype())).first;
								
								ait->second.places[k] += 1;
							}
						}


// 						auto sit = stats.find(votings[j].players[k]);
// 						sit->second.sumPlaces[k] += 1;
// 						double b = 3.0 - k; //3/2/1 балла
// 						sit->second.sum += b;
// 						//sit->second.av += b / n;

						std::vector<std::pair<std::string, pltype>> auxVec;
						for (const auto& pair : auxSums) auxVec.push_back(pair);


						for (int m = 0; m < auxVec.size(); ++m)
						{
							auto sit = stats.find(auxVec[m].first);
							for (int k = 0; k < 3; ++k)
							{
								double b = plsc[k];
								sit->second.sum += auxVec[m].second.places[k] * b;
								sit->second.sumPlaces[k] += auxVec[m].second.places[k];
							}
						}


#ifdef USE_MAX_METHOD
						for (auto& pair : auxVec) pair.second.s = 0;
						std::sort(auxVec.begin(), auxVec.end(),
							[](const auto& p1, const auto& p2) { return p1.second > p2.second; });
						for (int k = 0; k < 3; ++k)
						{
							auto sit = stats.find(auxVec[k].first);
							sit->second.max += 3.0 - k;
							sit->second.maxPlaces[k] += 1;
					}
#endif

						for (auto& pair : auxVec)
						{
							pair.second.s = 0;
							for (int k = 0; k < 3; ++k)
							{
								double b = plsc[k];
								pair.second.s += pair.second.places[k] * b / n;
							}
						}
						std::sort(auxVec.begin(), auxVec.end(),
							[](const auto& p1, const auto& p2) { return p1.second.s > p2.second.s; });
						for (int m = 0; m < auxVec.size(); ++m)
						{
							auto sit = stats.find(auxVec[m].first);
							sit->second.av += auxVec[m].second.s;
							if (m < 3) sit->second.avPlaces[m] += 1;
						}

						j0 = j1;
				}
			}

				i0 = i1;
			}

		}

	}
	else {
		log += "Error: Could not open the file " + dataFileName + "\n";
	}

	std::map<int, int> votInTourNum;
	for (auto& vot : votings) 
	{
		auto it = votInTourNum.find(vot.tour);
		if (it != votInTourNum.end()) it->second += 1;
		else votInTourNum[vot.tour] = 1;
	}


	std::ofstream outf(resultFileName);

	outf << "CALL:\n";
	for (int i = 0; i < argc; ++i) outf << argv[i] << "  ";

	outf << "\n\nTEAMS:\n";
	for (auto& team : teams) outf << team << "  ";

	outf << "\n\nLOG:\n" << log;

	std::vector<statpair> statsVec;
	for (auto& st : stats) statsVec.push_back(st);

	std::sort(statsVec.begin(), statsVec.end(), 
		[](statpair const& a, statpair const& b) { return comparestat(a, b, 0); });

	outf << "\nALL TOURS N = " << votInTourNum.size() << " (в скобках количество голосований в туре):\n";
	for (auto& t : votInTourNum) outf << t.first << "(" << t.second << ")  ";
	outf << std::endl;

	outf << "\nголосовавшие N = " << includeVotMans.size() << " (в скобках количество голосований): ";
	for (auto& voter : includeVotMans) outf << voter.first << "(" << voter.second.n << ")  ";
	outf << std::endl;

	outf << "\nисключенные из голосовани€ N = " << excludeVotMans.size() << " : ";
	for (auto& voter : excludeVotMans) outf << voter << "  ";
	outf << std::endl;


	std::string votToursStr = getToursString(votingTours);
	outf << "\nSUM METHOD (туры " << votToursStr << ")" << std::endl;
	outf << std::setw(4) << "є" << std::setw(15) << "им€" << "  " << std::setw(8) << "баллы" << "  1ые  2ые  3ьи" << std::endl;
	int c = 1;
	for (auto it = statsVec.begin(); it != statsVec.end(); ++it)
	{
		outf << std::setw(4) << c;
		writeplayer(outf, *it, 0);
		c++;
	}

	std::sort(statsVec.begin(), statsVec.end(),
		[](statpair const& a, statpair const& b) { return comparestat(a, b, 1); });

	c = 1;
	outf << "\nAVERAGE METHOD (туры " << votToursStr << ")" << std::endl;
	outf << std::setw(4) << "є" << std::setw(15) << "им€" << "  " << std::setw(8) << "баллы" << "  1ые  2ые  3ьи" << std::endl;
	for (auto it = statsVec.begin(); it != statsVec.end(); ++it)
	{
		outf << std::setw(4) << c;
		writeplayer(outf, *it, 1);
		c++;
	}

#ifdef USE_MAX_METHOD
	std::sort(statsVec.begin(), statsVec.end(),
		[](statpair const& a, statpair const& b) { return comparestat(a, b, 2); });

	c = 1;
	outf << "\MAX BY MATCH METHOD (туры " << votToursStr << ")" << std::endl;
	outf << std::setw(4) << "є" << std::setw(15) << "им€" << "  " << std::setw(8) << "баллы" << "  1ые  2ые  3ьи" << std::endl;
	for (auto it = statsVec.begin(); it != statsVec.end(); ++it)
	{
		outf << std::setw(4) << c;
		writeplayer(outf, *it, 2);
		c++;
	}
#endif


	outf.close();







// 	std::string fileName = "F:/Home/Src/best/data.txt";
// 	std::wifstream wif(fileName);
// 	wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
// 
// 	// Check if the file was successfully opened
// 	if (wif.is_open()) {
// 		std::wstring line;
// 		// Read the file line by line
// 		while (std::getline(wif, line)) {
// 
// 			ltrim(line);
// 
// 			if (line.empty() || line[0] == L'#')
// 				continue;
// 
// 			std::vector<std::wstring> words = getWords(line);
// 
// 			if (words[0] == L"команды:")
// 			{
// 				double rrr = 0;
// 			}
// 
// 		}
// 		wif.close(); // Close the file
// 	}
// 	else {
// 		std::cout << "Error: Could not open the file " << fileName << std::endl;
// 	}
}
