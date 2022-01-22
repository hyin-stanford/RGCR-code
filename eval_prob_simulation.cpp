#include "random_clustering/random_clustering.h"
#include "RGCR.h"



int main(int argc, char **argv) {
  reset_random_seed();
  std::cout << "Program start..." << std::endl;
  std::cout << rand() << ' ' << rand() << ' ' << rand() << std::endl << std::endl;

  std::string usage_msg = "Usage: ./" + std::string(argv[0]);
  usage_msg += " -g path_graph_name[synthetic/small_world/SW-2D_32_1_PL-id]";
  usage_msg += " -c clustering_method[nbr_max-1]";
  usage_msg += " -w clustering_weight[uniform]";
  usage_msg += " -s file_suffix[3-0.txt]";
  usage_msg += " -r use_complete_rand[false]";
  usage_msg += " -h estimator_type[Hajek]";
  usage_msg += " -n n_rounds[1]";
  usage_msg += " -a base_response[1]";
  usage_msg += " -b drift_std[0.5]";
  usage_msg += " -e noise_std[0.1]";
  usage_msg += " -t GATE[1]";
  usage_msg += " -m multiplicative_ATE[false]";
  usage_msg += " -o output_file_suffix[""]";

  // Default run parameters.
  std::string path_graph_name = "synthetic/small_world/SW-2D_32_1_PL-id";
  std::string clustering_method = "nbr_max-1";
  std::string clustering_node_w_opt = "uniform";
  std::string file_suffix = "3-0.txt";
  bool use_complete_rand = false;
  std::string est_type_str = "Hajek";
  int n_rounds = 1;
  double a = 1;
  double b = 0.5;
  double e = 0.1;
  double GATE = 1.0;
  bool additive_ATE = true;
  std::string output_file_suffix = "";

  extern char *optarg;
  int opt;
  while ((opt = getopt(argc, argv, "g:c:w:s:rh:n:a:b:e:t:mo:")) != -1) {
    switch (opt) {
      case 'g':
        path_graph_name = optarg;
        break;
      case 'c':
        clustering_method = optarg;
        break;
      case 'w':
        clustering_node_w_opt = optarg;
        break;
      case 's':
        file_suffix = optarg;
        break;
      case 'r':
        use_complete_rand = true;
        break;
      case 'h':
        est_type_str = optarg;
        break;
      case 'n':
        n_rounds = atoi(optarg);
        break;
      case 'a':
        a = atof(optarg);
        break;
      case 'b':
        b = atof(optarg);
        break;
      case 'e':
        e = atof(optarg);
        break;
      case 't':
        GATE = atof(optarg);
        break;
      case 'm':
        additive_ATE = false;
        break;
      case 'o':
        output_file_suffix = optarg;
        break;
      default:
        std::cout << usage_msg << std::endl;
        return -1;
    }
  }

  std::string run_name = path_graph_name + ',' + clustering_method + '-' + clustering_node_w_opt + ',' + file_suffix + ',';
  if (use_complete_rand) {
    run_name += "com_randomization,";
  } else {
    run_name += "ind_randomization,";
  }
  run_name += est_type_str + ',';
  run_name += std::to_string(n_rounds) + ',';
  run_name += std::to_string(a) + ',' + std::to_string(b) + ',' + std::to_string(e);
  if (additive_ATE) {
    run_name += ",additive_TE,";    
  } else {
    run_name += ",multipli_TE,";
  }
  run_name += std::to_string(GATE);
  std::cout << run_name << std::endl;

  std::cout << get_time_str() << ": Experiment starts..."<< std::endl;
  PUNGraph g = read_undirected_graph(GRAPH_CORE_PATH + path_graph_name);
  RGCR rgcr(g, path_graph_name, false);
  rgcr.load_node_response(a, b, e, GATE, additive_ATE);

  std::string output_file_name = "simulation-" + est_type_str + output_file_suffix + ".txt";
  std::ofstream file_out(output_file_name, std::ofstream::app);
  file_out << run_name << std::endl;
  RandomClustering random_clustering(g, path_graph_name, clustering_method, clustering_node_w_opt);
  rgcr.eval_expo_prob_simulation_stratified(random_clustering, n_rounds, use_complete_rand, file_suffix, est_type_str, file_out);
  file_out.close();

  std::cout << get_time_str() << ": Experiment finishes..."<< std::endl;
  return 0;
}
