#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <queue>
#include <map>
#include <fstream>

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
    HuffmanEncoder(priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> &q, string message): message(message){
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

    void show_content(NodeHuffman *node) const{

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

    void serialize_tree(string filepath, string filepath2) const{
        ofstream writer;
        writer.open(filepath);
        if(!writer) 
            throw runtime_error("Could not write file");
        
        writer.close();
        writer.open(filepath2);
        if(!writer)
            throw runtime_error("Could not write file");

        writer.close();
    }

    void serialize_node(NodeHuffman* node, string filepath) const{
        ofstream writer;
        writer.open(filepath);
        if (!writer) throw runtime_error("Could not write file");
        writer << node-> code << " " << node-> c << " " << node->freq << "\n";
        writer.close();
    }

};

struct HuffmanPreprocessor{

    string buffer;
    priority_queue<NodeHuffman *, vector<NodeHuffman *>, Compare> frequencies;

    HuffmanPreprocessor(string content) : buffer(content), frequencies(get_frequencies()) {}

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
    string pre_order_filepath;
    string symetric_order_filepath;
    HuffmanEncoder enc;

    HuffmanDecoder(HuffmanEncoder enc, string filepath): enc(enc), filepath(filepath){}
    HuffmanDecoder(string pre, string sym, string filepath): filepath(filepath), pre_order_filepath(pre), symetric_order_filepath(sym) {
        enc = generate_tree(pre, sym);
    }

    HuffmanEncoder generate_tree(string pre, string sym){

    }

    void decode_message(){
        ifstream reader;
        reader.open(filepath, ios::binary);
        if(!reader) throw runtime_error("Could not found file");
        // reader.read()
        reader.close();
    }
};



int main(){
    string s = "The Zen of Python, by Tim Peters\n\nBeautiful is better than ugly.\nExplicit is better than implicit.\nSimple is better than complex.\nComplex is better than complicated.\nFlat is better than nested.\nSparse is better than dense.\nReadability counts.\nSpecial cases aren't special enough to break the rules.\n Although practicality beats purity.\nErrors should never pass silently.\nUnless explicitly silenced.\nIn the face of ambiguity, refuse the temptation to guess.\nThere should be one-- and preferably only one-- obvious way to do it.\nAlthough that way may not be obvious at first unless you're Dutch.\n Now is better than never.\nAlthough never is often better than * right * now.\nIf the implementation is hard to explain, it's a bad idea.\n If the implementation is easy to explain, it may be a good idea.\nNamespaces are one honking great idea-- let's do more of those!"; 
    HuffmanPreprocessor alfa(s);
    HuffmanEncoder beta(alfa.frequencies, s);
    beta.encode_message("zenofpython.bin");
    return 0;
}
