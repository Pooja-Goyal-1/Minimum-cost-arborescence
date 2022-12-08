#include<bits/stdc++.h>
using namespace std;

// Function to check if all the vertices are reachable from the given root using bfs.
// Arborescence exits only if all the vertices are reachable from root. 
void isValid(vector<int> graph[], int root, int n){
    
    //running bfs
    vector<bool> visited(n + 1, false);
    queue<int> queue; 
    queue.push(root);
    visited[root] = true;
    while (!queue.empty()){
        int x = queue.front();
        queue.pop();
        for (int i = 1; i <= n; i++){
            if ((graph[x][i] != -1) && (!visited[i])){
                visited[i] = true;
                queue.push(i);
            }
        }
    }

    //if graph is not connected then arboroscence does not exists
    for (int i = 1; i <= n; i++){
        if (!visited[i]){
            cout <<"The Graph is not connected\n";
            exit(0);
        }
    }

}

//algorithm for minimum cost arboroscence
void min_cost_arboroscence(vector<int> graph[], int n){
    
    vector<int> contracted_edges_graph[n + 1]; // graph with reduced edges
    vector<int> min_inedge[n + 1]; // graph containing atmost one zero inedge (if it exists)
    vector<int> next_subgraph[n + 1];
    vector<int> min_weight(n + 1, -1); //stores minimum in-edge weight for all vertices

    // initializing contracted_edges_graph and min_inedge
    for (int i = 0; i <= n; i++){
        for (int j = 0; j <= n; j++){
            contracted_edges_graph[i].push_back(-1);
            min_inedge[i].push_back(-1);
        }
    }

    //To find min in-edge weight for all vertices
    for(int i = 1; i<=n; i++){
        for(int j=1;j<=n;j++){
            if(graph[i][j]!=-1){
                if (min_weight[j] == -1){
                    min_weight[j] = graph[i][j];
                }
                else{
                    min_weight[j] = (graph[i][j] < min_weight[j]) ? graph[i][j] : min_weight[j];
                }
            }
        }
    }

    //reducing edges by subtracting min-in-edge from all in edges
    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            if (graph[i][j] != -1){
                contracted_edges_graph[i][j] = graph[i][j] - min_weight[j];
            }
        }
    }

    //constructing the zero in-edge graph from reduced graph
    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            if (contracted_edges_graph[j][i] == 0){
                min_inedge[j][i] = 0;
                break;
            }
        }
    }

    //dfs is applied to detect cycle in zero-edge graph
    vector<int> color(n + 1, 0);
    stack<int> stack;

    stack.push(1);
    color[1]=1;
    int fg = 0;
    bool cycle_exist = false;
    int cycle_last = 0;

    while (!stack.empty()){
        int u = stack.top();
        fg = 0;
        for (int v = 1; v <= n; v++){
            if (min_inedge[u][v] != -1){
                if (!color[v]){
                    color[v] = 1;
                    stack.push(v);
                    fg = 1;
                    break;
                }

                else if (color[v] == 1){ //cycle exists
                    cycle_exist = true;
                    cycle_last = v;
                    break;
                }
            }
        }
        if (cycle_exist){
            break;
        }
        if (fg){
            continue;
        }
        stack.pop(); 
        color[u] = 2; 

        if (stack.empty()){
            for (int i = 1; i <= n; i++){
                if (!color[i]){
                    stack.push(i);
                    color[i] = 1;
                    break;
                }
            }
        }
    }


    if (cycle_exist){
        set<int> compressed_nodes; //has the nodes forming the cycle
        while (stack.top() != cycle_last){
            compressed_nodes.insert(stack.top());
            stack.pop();
        }
        compressed_nodes.insert(cycle_last);

        int supernode_index = *(compressed_nodes.begin()); 

        //next_subgraph is filled by contracted_edges_graph
        for (int i = 0; i <= n; i++){
            for (int j = 0; j <= n; j++){
                next_subgraph[i].push_back(contracted_edges_graph[i][j]);
            }
        }

        vector<int> compressed_in(n + 1, -1);  //to maintain to which node in the cycle was there an edge that was selected in supernode
        vector<int> compressed_out(n + 1, -1); //to maintain from which node in the cycle was there an edge that was selected in supernode
        set<int>::iterator it;
        //forming supernode
        for (it = compressed_nodes.begin(); it != compressed_nodes.end(); it++){
            int u = *it;
            for (int v = 1; v <= n; v++){
                if (compressed_nodes.find(v) == compressed_nodes.end()){ //nodes in the cycle need not be considered
                    //forming out edges of supernode
                    if (next_subgraph[u][v] != -1)
                    {
                        if (next_subgraph[supernode_index][v] == -1){
                            next_subgraph[supernode_index][v] = next_subgraph[u][v];
                            compressed_out[v] = u;
                        }
                        else if (next_subgraph[supernode_index][v] > next_subgraph[u][v]){
                            next_subgraph[supernode_index][v] = next_subgraph[u][v];
                            compressed_out[v] = u; //storing the index of node in cycle whose out_edge is selected
                        }
                    }
                    //forming in edges of supernode
                    if (next_subgraph[v][u] != -1){
                        if (next_subgraph[v][supernode_index] == -1){
                            next_subgraph[v][supernode_index] = next_subgraph[v][u];
                            compressed_in[v] = u;
                        }
                        else if (next_subgraph[v][supernode_index] > next_subgraph[v][u])
                        {
                            next_subgraph[v][supernode_index] = next_subgraph[v][u];
                            compressed_in[v] = u; //storing the index of node in cycle whose in_edge is selected
                        }
                    }
                }
            }
        }

        //disconnecting all nodes of the cycle in next_subgraph
        for (it = compressed_nodes.begin(); it != compressed_nodes.end(); it++){
            int u = *it;
            for (int v = 1; v <= n; v++){
                if (u != supernode_index){
                    next_subgraph[u][v] = -1;
                    next_subgraph[v][u] = -1;
                }
            }
        }

        min_cost_arboroscence(next_subgraph, n);//calling function recursively on new grapg(next_subgraph)

        //expanding the supernode using compressed_in and compressed_out
        for (int v = 1; v <= n; v++){
            if (compressed_in[v] != -1){
                next_subgraph[v][compressed_in[v]] = next_subgraph[v][supernode_index];
                //disconnecting from the supernode_index vertex if it was not a connection initially
                if (compressed_in[v] != supernode_index){
                    next_subgraph[v][supernode_index] = -1;
                }
            }

            if (compressed_out[v] != -1){
                next_subgraph[compressed_out[v]][v] = next_subgraph[supernode_index][v];
                if (compressed_out[v] != supernode_index){
                    next_subgraph[supernode_index][v] = -1;
                }
            }
        }

        //selecting edges from supernode (cycle) to form arboroscence
        for (it = compressed_nodes.begin(); it != compressed_nodes.end(); it++){
            int u = *it;
            bool inedge_flag = false; 
            for (int v = 1; v <= n; v++){
                if (next_subgraph[v][u] != -1){
                    inedge_flag = true;
                    break;
                }
            }

            //If node doesn't have an in-edge, then select in-edge from the zero-edge cycle for that node
            if (!inedge_flag){
                for (int v = 1; v <= n; v++){
                    if (min_inedge[v][u] != -1){
                        next_subgraph[v][u] = min_inedge[v][u];
                    }
                }
            }
        }

    }
    //if cycle is not present the min_inedge graph contains the final edges of arboroscence
    if (!cycle_exist){
        for (int i = 0; i <= n; i++){
            for (int j = 0; j <= n; j++){
                next_subgraph[i].push_back(min_inedge[i][j]);
            }
        }
    }

    //using subgraph remove the edges from original graph which are not part of the minimum arboroscence
    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            if(next_subgraph[i][j] == -1){
                graph[i][j] = -1;
            }
        }
    }


}

//Function to print the edges of minimum arboroscence and finding cost
void printArboroscence(vector<int> graph[], int n)
{
    int cost = 0;            //total cost
    cout<<"Th minimum arboroscence has the following edges: \n";
    //To calculate total cost and print the edges in the minimum arboroscence
    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            if (graph[i][j] != -1){
                cout<< i << " " << j << " " << graph[i][j] << endl;
                cost += graph[i][j];
            }
        }
    }
    
    cout<< "Total cost of the minimum arboroscence comes out as: " << cost << endl; 
    cout<<endl;
}


int main(){
    
    int t; //number of testcases
    cout<< " Enter the number of testcases: ";
    cin >> t ;
    while (t--)
    {
        int n, m, root;
        cout<<"Enter number of vertices: ";
        cin >> n ;
        cout<<"Enter number of edges: ";
        cin >> m ;
        cout<<"Enter source vertex: ";
        cin >> root ;

        vector<int> graph[n + 1]; //graph stored as adjacency matrix

        for (int i = 0; i <= n; i++){
            for (int j = 0; j <= n; j++){
                graph[i].push_back(-1);
            }
        }

        //To take input for all edges
        cout<<"Enter the edges and weights (start vertex, end vertex, weight): \n";
        while (m--){
            int u, v, weight;
            cin >> u >> v >> weight;

            //taking only minimum edge from u to v
            if (graph[u][v] == -1)
                graph[u][v] = weight;

            else if (weight < graph[u][v])
                graph[u][v] = weight;
        }

        // Removing all the inedges to the root vertex since these won't be counted in arborescence.
        for (int i = 1; i <= n; i++){
            graph[i][root] = -1;
        }

        //Pre process the graph (to check if every vertex is reachable from the root)
        isValid(graph, root, n);

        //To run Minimum Cost Arboroscence algorithm
        min_cost_arboroscence(graph, n);

        //To print the minimum arboroscence
        printArboroscence(graph, n);
    }

    return 0;
}