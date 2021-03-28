#include <c++/9/algorithm>
#include <c++/9/iostream>
#include <c++/9/map>
#include <c++/9/set>
#include <c++/9/string>
#include <c++/9/utility>
#include <c++/9/vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

struct Query{
	vector<string> plusWord;
	vector<string> minusWord;
};

string ReadLine() {
		string s;
		getline(cin, s);
		return s;
}

int ReadLineWithNumber() {
		int result;
		cin >> result;
		ReadLine();
		return result;
}

vector<string> SplitIntoWords(const string& text) {
		vector<string> words;
		string word;
		for (const char c : text) {
				if (c == ' ') {
						words.push_back(word);
						word = "";
				} else {
						word += c;
				}
		}
		words.push_back(word);

		return words;
}

struct Document {
		int id;
		int relevance;
};

class SearchServer {
public:
		void SetStopWords(const string& text) {
				for (const string& word : SplitIntoWords(text)) {
						stop_words_.insert(word);
				}
		}

		void AddDocument(int document_id, const string& document) {
				for (const string& word : SplitIntoWordsNoStop(document)) {
						word_to_documents_[word].insert(document_id);
				}
		}

		vector<Document> FindTopDocuments(const string& query) const {
				auto matched_documents = FindAllDocuments(query);

				sort(
						matched_documents.begin(),
						matched_documents.end(),
						[](const Document& lhs, const Document& rhs) {
								return lhs.relevance > rhs.relevance;
						}
				);
				if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
					matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
				}
				return matched_documents;
		}

private:
		map<string, set<int>> word_to_documents_;
    set<string> stop_words_;

		vector<string> SplitIntoWordsNoStop(const string& text) const {
				vector<string> words;
				for (const string& word : SplitIntoWords(text)) {
						if (stop_words_.count(word) == 0) {
								words.push_back(word);
						}
				}
				return words;
		}

		vector<Document> FindAllDocuments(const string& query) const {
			    Query query_parse = ParseQuery(query);
			    //const vector<string> query_words = SplitIntoWordsNoStop(query);
			    const vector<string> query_words = query_parse.plusWord;
			    const vector<string> minus_words = query_parse.minusWord;
			    map<int, int> document_to_relevance;
				for (const string& word : query_words) {
					if (word_to_documents_.count(word) == 0){
						continue;
					}else{
						for(const string& s: minus_words){
							if(s.compare(word)==0){
								for (const int document_id : word_to_documents_.at(word)) {
									++document_to_relevance[document_id];
								}
							}
						}

					}
				}

				vector<Document> matched_documents;
				for (auto [document_id, relevance] : document_to_relevance) {

						matched_documents.push_back({document_id, relevance});
				}

				return matched_documents;
		}
		Query ParseQuery(const string& query) const{
			Query qr;
			vector<string> spWords = SplitIntoWordsNoStop(query);
			for(const string s: spWords){
				if(s.at(0) == '-'){
					qr.minusWord.push_back(s.substr(1));
				}else{
					qr.plusWord.push_back(s);
				}
			}
			return qr;
		}
};

SearchServer CreateSearchServer() {
		SearchServer search_server;
		search_server.SetStopWords(ReadLine());

		const int document_count = ReadLineWithNumber();
		for (int document_id = 0; document_id < document_count; ++document_id) {
				search_server.AddDocument(document_id, ReadLine());
		}

		return search_server;
}


int main() {
		const SearchServer search_server = CreateSearchServer();

		const string query = ReadLine();
		for (auto [document_id, relevance] : search_server.FindTopDocuments(query)) {
				cout << "{ document_id = " << document_id << ", relevance = " << relevance << " }" << endl;
		}
}
