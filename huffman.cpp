#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include<sstream>

using namespace std;


struct NodeHuffman{
    size_t freq;
    char c;
    string code;
    NodeHuffman* left;
    NodeHuffman* right;

    NodeHuffman(size_t freq, char ch): freq(freq), c(ch), code(), left(), right() {}
    ~NodeHuffman() {
        left = nullptr;
        right = nullptr;
    }

};


struct Compare{
    bool operator()(const NodeHuffman* a, const NodeHuffman* b) const{
        return a->freq > b->freq;
    }    
};


struct HuffmanEncoder{
    NodeHuffman* root;
    string message;
    HuffmanEncoder(): root(nullptr), message("") {}
    HuffmanEncoder(priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> &q, string message=""): message(message){
        // if(q.size() == 0) throw runtime_error("queue is empty");
        if(q.size() == 1) throw runtime_error("pointless use of Huffman algorithm");
        while(!q.empty()){
            if(q.size() == 1){
                root = q.top(); 
                q.pop();
                break;
            }
            NodeHuffman* first = q.top(); q.pop();
            NodeHuffman* second = q.top(); q.pop();
            NodeHuffman* aux = new NodeHuffman(first->freq+second->freq, '\0');
            aux->left = first; aux->right = second;
            q.push(aux);
        }
        if(!code_emplacing()) throw runtime_error("Could not code Huffman tree nodes");
    }
    
    HuffmanEncoder(HuffmanEncoder& other): root(other.root), message(other.message) {
        other.root = nullptr;
    }

    HuffmanEncoder& operator=(HuffmanEncoder& other){
        root = other.root; 
        message = other.message;
        other.root = nullptr;
    }
    HuffmanEncoder& operator=(HuffmanEncoder&& other) {
        root = other.root;
        message = other.message;
        other.root = nullptr;
    }

    ~HuffmanEncoder() {
        // Delete tree
        if(root == nullptr) return;
        else delete_node(root);
    }

    bool code_emplacing(){
        return code_assignment(root, "");
    }

    bool code_assignment(NodeHuffman* tracker, string code){
        if(tracker->left == nullptr && tracker->right == nullptr){
            tracker->code = code;
            tracker->code.shrink_to_fit();
            return true;
        } else if(tracker->right == nullptr){
            tracker->code.shrink_to_fit();
            return code_assignment(tracker->left, code+"0");
        } else if(tracker->left == nullptr){
            tracker->code.shrink_to_fit();
            return code_assignment(tracker->right, code+"1");
        } else{
            tracker->code.shrink_to_fit();
            bool b1 = code_assignment(tracker->left, code+"0");
            bool b2 = code_assignment(tracker->right, code+"1");
            return b1 && b2;
        }
    }

    void show_table() const{

    }

    NodeHuffman* get_root(){
        return root;
    }

    string query(char c, NodeHuffman* current) const{
        if(current == nullptr) return "\0";
        if(current->c == c) return current->code;
        else{
            string aux1 = query(c, current->left);
            string aux2 = query(c, current->right);
            return aux1=="\0" ? aux2 : aux1;
        }
    }

    void encode_message(string filepath) const{
        ofstream writer;
        writer.open(filepath);
        if(!writer) throw runtime_error("can't open file " + filepath + " for writing");
        string aux = encoded_msg_string();
        writer.write(aux.c_str(),aux.size());  
        writer.close();
    }

    void encode_binary(string filepath) const{ // TODO: fix this method
        const int chunk_size = 64;
        vector<bitset<chunk_size>> bit_chunks;
        for (int i = 0; i < message.length(); i += 64){
            string chunk_str = message.substr(i, 64);
            bit_chunks.emplace_back(bitset<chunk_size>(chunk_str));
        }
        ofstream outfile(filepath, ios::binary);
        for (const auto &chunk : bit_chunks){
            unsigned long long value = chunk.to_ullong();                   
            outfile.write(reinterpret_cast<char *>(&value), sizeof(value)); 
        }
        outfile.close();
    }

    string encoded_msg_string() const{
        string aux;
        for(auto c:message){
            aux+=query(c,root);
        }
        return aux;
    }

    void serialize_tree(string filepath) const{
        ofstream writer;
        writer.open(filepath,ios::out);
        if(!writer) 
            throw runtime_error("Could not write file");
        serialize_pre_order(root, writer);
        writer.close();
        // writer.open(filepath2);
        // if(!writer)
        //     throw runtime_error("Could not write file");

        // writer.close();
    }

    private:
        
        void show_table_single_node(NodeHuffman *node) const{

        }

        void serialize_node(NodeHuffman* node, ofstream& writer) const{
            if(node->c=='\0') return;
            writer << node-> c << " " << node->freq << "\n";
        }

        void serialize_pre_order(NodeHuffman* node, ofstream& writer) const{
            serialize_node(node, writer);
            if(node->left != nullptr) serialize_pre_order(node->left, writer);
            if(node->right != nullptr) serialize_pre_order(node->right, writer);
        }

        void serialize_sym_order(NodeHuffman* node, ofstream& writer) const{
            if(node->left != nullptr) serialize_sym_order(node->left, writer);
            serialize_node(node, writer);
            if(node->right != nullptr) serialize_sym_order(node->right, writer);
        }

        void delete_node(NodeHuffman* node){
            if(node->left !=nullptr) delete_node(node->left);
            if(node->right !=nullptr) delete_node(node->right);
            delete node;
        }

};

struct HuffmanPreprocessor{

    string buffer;
    priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> frequencies;

    HuffmanPreprocessor(string content) : buffer(content), frequencies(get_frequencies()) {}

    HuffmanPreprocessor(map <char,int> freq) : buffer(""), frequencies(get_frequencies_decoder(freq)) {}

    priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> get_frequencies(){
        priority_queue<NodeHuffman*, vector<NodeHuffman*>, Compare> pq;
        map<char,size_t> counter;
        for(auto c: buffer){
            counter[c]++;
        }
        for (auto pr: counter){
            pq.push(new NodeHuffman(pr.second, pr.first));
        }
        return pq;
    }

    priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> get_frequencies_decoder(map <char,int> freq){
        priority_queue<NodeHuffman*, vector<NodeHuffman*>, Compare> pq;
        for (auto x: freq){
            pq.push(new NodeHuffman(x.second, x.first));
        }
        return pq;
    }
};


ostream& operator<<(ostream& os, const HuffmanEncoder& encoder){
    os << encoder.encoded_msg_string();
    return os;
}


size_t getsizeof(NodeHuffman& enc){
    size_t aux = sizeof(enc) + sizeof(enc.code.c_str());
    if(enc.left != nullptr) aux += sizeof(enc.left);
    if(enc.right != nullptr) aux += sizeof(enc.right);
    return aux;
}

size_t getsizeof(HuffmanEncoder& enc){
    return getsizeof(*(enc.root)) + sizeof(enc);
}



struct HuffmanDecoder{
    string filepath;
    string freq_filepath;
    map <char,int> freq_map;
    string message;
    string message_filepath;
    HuffmanEncoder enc;

    HuffmanDecoder(HuffmanEncoder enc, string filepath): enc(enc), filepath(filepath){}

    HuffmanDecoder(string freq, string filepath): freq_filepath(freq),filepath(filepath){
        ifstream arqfreq(freq);
        if(arqfreq.is_open()){
            char c,e;
            int n;
            while(1){
                arqfreq.get(c);
                arqfreq>>n;
                arqfreq.get(e);
                if(arqfreq.eof()) break;
                freq_map[c]=n;
            }
            arqfreq.close();
        }
        else{
            throw runtime_error("nao foi possivel abrir o arquivo");
        }
        HuffmanPreprocessor pre_huff(freq_map);
        enc=HuffmanEncoder(pre_huff.frequencies);
    }

    void decoder(){
        ifstream reader(filepath);
        ofstream writer;
        writer.open("message.txt",ios::out);
        if(!writer.is_open()){
            throw runtime_error("nao foi possivel abrir o arquivo");
        }
        string line;
        if(reader.is_open()){
            getline(reader,line);
        }
        else{
            throw runtime_error("nao foi possivel abrir o arquivo");
        }
        NodeHuffman *root=enc.get_root(),*aux;
        aux=root;
        int index=0;
        string::iterator it=line.begin();
        while(it!=line.end()){
            if(aux->c!='\0'){
                writer<<aux->c;
                aux=root;
                continue;
            }
            if(*it=='0') aux=aux->left;
            else aux=aux->right;
            it++;
        }
        writer<<aux->c;
    }
};


/*
arquivo so com 0001110001010
atraves da ordem remonta o grafo de letras 




*/


int main(){
    string s = "The Zen of Python, by Tim Peters\n\nBeautiful is better than ugly.\nExplicit is better than implicit.\nSimple is better than complex.\nComplex is better than complicated.\nFlat is better than nested.\nSparse is better than dense.\nReadability counts.\nSpecial cases aren't special enough to break the rules.\n Although practicality beats purity.\nErrors should never pass silently.\nUnless explicitly silenced.\nIn the face of ambiguity, refuse the temptation to guess.\nThere should be one-- and preferably only one-- obvious way to do it.\nAlthough that way may not be obvious at first unless you're Dutch.\n Now is better than never.\nAlthough never is often better than * right * now.\nIf the implementation is hard to explain, it's a bad idea.\n If the implementation is easy to explain, it may be a good idea.\nNamespaces are one honking great idea-- let's do more of those!"; 
    HuffmanPreprocessor alfa(s);
    HuffmanEncoder beta(alfa.frequencies, s);
    beta.encode_message("zenofpython.txt");
    beta.serialize_tree("tree.txt");
    HuffmanDecoder a("tree.txt","zenofpython.txt");
    a.decoder();
    return 0;
}
