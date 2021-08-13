/*
 * GA.cpp
 *
 *  Created on: 21/06/2015
 *      Author: Carla Santana
 */

#include "GeneticAlgorithm.h"

GeneticAlgorithm *GeneticAlgorithm::instance = NULL;

/*
 * Construtor GA:
 * Inicializa o nulo e nullo
 * calcula o primeiro tempo de ciclo
 */
GeneticAlgorithm::GeneticAlgorithm(Line* assemblyLine){
	this->assemblyLine = assemblyLine;

	this->allTask = assemblyLine->getTasks();
	this->nulo.push_back(-10);
	vector<double> n;
	n.push_back(-10);
	this->nullo = new Individual(nulo, nulo, nulo, nulo,n,0);
	this->factivel = nullo;
	double temp =0;
	for (int t = 0; t < assemblyLine->getN(); t++) {
		temp+= allTask[t]->getAvgExecutionTime(assemblyLine->getK(), assemblyLine->getM());
	}
	this->cycleTime = temp/assemblyLine->getK();
	this->tableTasks.resize(assemblyLine->getN());
	for (int p = 0; p < assemblyLine->getN(); p++){
		tableTasks[p].resize(assemblyLine->getN());
	}
	// calcula o tempo medio de cada tarefa para cada trabalhador e coloca o a latest station = -1
	// e a earliest station = K
	assemblyLine->prepareTasksForGA();

}

GeneticAlgorithm::~GeneticAlgorithm() {
	delete assemblyLine;
	//delete tableTasks;
}

/*
 * Retorna true se o vetor possui o content, false caso contrario
 */
bool containsContent(vector<int> v, int content){
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == content) {
			return true;
		}
	}
	return false;
}

// GeneticALgorithm é um singleton
GeneticAlgorithm* GeneticAlgorithm::getInstance(Line* assemblyLine)
{
   if (!instance)
      instance = new GeneticAlgorithm(assemblyLine);

   return instance;
}

/**
 * Gera a matriz com o grafo de precedencia das tarefas
 */
void GeneticAlgorithm::generateTableTasks(){

	for (int p = 0; p < assemblyLine->getN(); p++) {
		for (int f = 0; f < assemblyLine->getN(); f++) {
			if (containsSucessors(p,f)) {
				tableTasks[p][f] =1;
			}else{
				tableTasks[p][f] =0;
			}
		}
	}
}

/*
 * Printa a matriz de grafo de precedencia das tarefas
 */
void GeneticAlgorithm::printTableTasks(){

	for (int p = 0; p < assemblyLine->getN(); p++) {

		for (int f = 0; f < assemblyLine->getN(); f++) {
			cerr << "|" << tableTasks[p][f] << "|";

		}
		cerr << " Linha " << p << endl;
	}
}

/**
 * Consulta as tarefas para analisar se f é sucessor de p
 */
bool GeneticAlgorithm::containsSucessors(int p, int f){

	allTask[p]->getImmediateSuccessorsBeginIterator();
	for (vector<Task*>::const_iterator it = allTask[p]->getImmediateSuccessorsBeginIterator();
						it != allTask[p]->getImmediateSuccessorsEndIterator(); ++it) {
		if ((*it)->getId() == f) {
			return true;
		}
	}
	return false;
}

/**
 * Gera a poulação inicial
 */
void GeneticAlgorithm::generateInicialPop(int qtdI){
	this->sizePop = qtdI;
	while(population.size() < qtdI){
		Individual* b = generateIndividual();
		if (!b->getInfeasible()) {
			population.push_back(b);
			if (factivel == nullo) {
				factivel = b;
			}else if(b->getFitness() < factivel->getFitness()){
				factivel = b;

			}
		}
	}
}

/**
 * Retorna o vetor do cruzamento de p1 e p2, pegando a parte do meio do p1 e restante do p2
 */
vector<int> GeneticAlgorithm::generateFModels(vector<int> p1, vector<int> p2){

	int tam= p1.size()/2; // tamanho que vai herda do pai 1
	int limiteInferior = tam - (tam-1)/2; //calculo do primeiro ponto que vai pegar do p1
	int limiteSuperior = tam + (tam-1)/2; //calculo do ultimo ponto que vai pegar do p1
	vector<int> qtdModels;
	qtdModels.resize(assemblyLine->getM());
	int soma=0;
	vector<int> f;
	f.resize(p1.size(),-1);

	//For para inserir no filho elementos do pai 1
	for (int i = limiteInferior; i <= limiteSuperior; i++) {

		f[i] = p1[i];
		// for para contabilizar a qtd de elementos que deve ser deletado do p2
		for (int j = 0; j < assemblyLine->getM(); j++) {
			if (f[i]==j) {
				qtdModels[j]++;
				soma++;
			}
		}
	}
	while(soma!=0) {
		int iDel = rand() % p1.size();
		for (int j = 0; j < assemblyLine->getM(); j++) {
			if (p2[iDel]==j && qtdModels[j]>0) {
				p2[iDel] = -1;
				qtdModels[j]--;
				soma--;
			}
		}
	}

	int i2=0;
	int i = 0;

	while (i < p1.size()){

		if (p2[i]!=-1) {

			f[i2]=p2[i];

			if(i2 + 1 == limiteInferior){
				i2 = limiteSuperior + 1;
			}else{
				i2++;
			}
		}
		i++;
	}
	return f;
}

/*
 * Retorna a qual estação a tarefa pertence naquele individuo
 */
int GeneticAlgorithm::stationOfTask(int task, int ind){

	for (int i = 0; i < population[ind]->getTasks().size(); i++) {
		if (population[ind]->getTasks()[i]==task) {
			return population[ind]->getStation()[i];
		}
	}
	return -1;
}

/**
 * Retorna a ultima estação que a tarefa pode ser colocada naquele individuo
 */
int GeneticAlgorithm::getLatestWorkstation(int task, int ind){

	int k = assemblyLine->getK()-1;

	for (vector<Task*>::const_iterator it = allTask[task]->getImmediateSuccessorsBeginIterator();
			it != allTask[task]->getImmediateSuccessorsEndIterator(); ++it){
		int ktemp = (*it)->getId();
		ktemp = stationOfTask(ktemp, ind);
		if (k > ktemp)
			k = ktemp;
	}
	return k;
}

/**
 * Retorna a primeira estação que a tarefa pode ser colocada naquele individuo
 */
int GeneticAlgorithm::getEarliestWorkstation(int task, int ind){

	int k = 0;

	for (vector<Task*>::const_iterator it = allTask[task]->getImmediatePredecessorsBeginIterator();
			it != allTask[task]->getImmediatePredecessorsEndIterator(); ++it){
		int ktemp = (*it)->getId();
		ktemp = stationOfTask(ktemp, ind);
		if (k < ktemp)
			k = ktemp;
	}
	return k;

}

/*
 * Gera mutação em qtdF individuos
 */
void GeneticAlgorithm::mutation(int porc){
	int qtdF = (sizePop*porc)/100;
	infacMutation=0;
	int i=0;
	while ( i < qtdF) {
		bool temp2 = mutationTask();
		if (temp2) {
			mutationModels();
			i++;
		}
	}

}

/*
 * Faz mutação de modelos nos individuos
 */
void GeneticAlgorithm::mutationModels(){

	int ind = rand()%population.size();

	int p1 = rand()%population[ind]->getModels().size();
	int p2 = rand()%population[ind]->getModels().size();

	vector<int> models = population[ind]->getModels();
	if (p2<p1) {
		int aux = p1;
		p1=p2;
		p2=aux;
	}

	int paux = p2;
	for (int p = p1; p <= p2; p++) {
		models[p] = population[ind]->getModels()[paux];
		paux--;
	}

	int tam = population[ind]->getTimeStation().size() -1;
	double t = population[ind]->getTimeStation()[tam];
	double wo = generateWo(population[ind]->getTasks(), population[ind]->getWorkers(), population[ind]->getStation(), models,t);
	Individual* mutante = new Individual(models, population[ind]->getWorkers(), population[ind]->getTasks(), population[ind]->getStation(),population[ind]->getTimeStation(),wo);
	population.push_back(mutante);
}

/*
 * Faz mutação de tarefas
 */
bool GeneticAlgorithm::mutationTask(){
	int ind = rand()%population.size();

	int p1 = rand()%assemblyLine->getN();
	int task1 = population[ind]->getTasks()[p1];
	int k1 = population[ind]->getStation()[p1];
	int e1 = getEarliestWorkstation(task1, ind);// earliestWorkstation(i);
	int l1 = getLatestWorkstation(task1,ind);

	while(l1 == e1){
		p1 = rand()%assemblyLine->getN();
		task1 = population[ind]->getTasks()[p1];
		k1 = population[ind]->getStation()[p1];
		e1 = getEarliestWorkstation(task1, ind);// earliestWorkstation(i);
		l1 = getLatestWorkstation(task1,ind);
	}
	int knew;
	if ((l1-e1-1)<=0) {

		if (k1 == e1) {
			knew = l1;
		}else if(k1==l1){
			knew = e1;
		}else{
			return false;
		}
	}else{
		knew = rand()%(l1-e1+1) + e1;
		while(knew == k1){
			knew = rand()%(l1-e1+1) + e1;
		}
		if (lastPositionStation(ind, knew) == -1) {
			return false;
		}
	}
	return moveTask(ind, k1, knew, p1);;
}

/*
 * Calcula qual ultima posição da estação  no individuo
 * exemplo:
 * 0 0 0 1 1 2 2 3 3
 * a ultima posição de 0 é 2
 */
int GeneticAlgorithm::lastPositionStation(int ind, int k){
	bool chegouK = false;
	for (int i = 0; i < population[ind]->getStation().size(); i++) {

		if (population[ind]->getStation()[i] == k) {
			chegouK = true;
		}else if(chegouK){
			return (i-1);
		}
	}
	return -1;
}

/*
 * Calcula qual primeira posição da estação  no individuo
 * exemplo:
 * 0 0 0 1 1 2 2 3 3
 * a primeira posição de 3 é 7
 */
int GeneticAlgorithm::earlyPositionStation(int ind, int k){
	bool chegouK = false;
	for (int i = (population[ind]->getStation().size()-1) ; i >=0 ; i--) {
		if (population[ind]->getStation()[i] == k) {
			chegouK = true;
		}else if(chegouK){
			return (i+1);
		}
	}
	return -1;
}

/*
 * Faz o move de tarefas, pega pTask e coloca na estação kNew de acordo ao inidividuo inicial ind
 */
bool GeneticAlgorithm::moveTask(int ind, int kOld, int kNew, int pTask){

	vector<int> task = population[ind]->getTasks();
	int taskV = task[pTask];
	if (kOld > kNew) {
		int lastPosStation = lastPositionStation(ind, kNew) +1;
		task[lastPosStation] = taskV;
		for (int i = lastPosStation+1; i <= pTask; i++) {
			task[i] = population[ind]->getTasks()[i-1];
		}
	}else{
		int earlyPosStation = earlyPositionStation(ind, kNew)-1;
		if (earlyPosStation == -2) {
			earlyPosStation = task.size() -1;
		}
		task[earlyPosStation] = taskV;
		for (int i = earlyPosStation-1; i >= pTask; i--) {
			task[i] = population[ind]->getTasks()[i+1];
		}
	}
	vector<int> newStations = updateStationMutation(kNew, kOld, ind, population[ind]->getStation());
	vector<int> wf = generateIndividualWorker(task, newStations);
	if (wf != nulo) {

		vector<double> timeStation = generateTimeStation(task, wf,newStations );
		double wo = generateWo(task, wf, newStations, population[ind]->getModels(), timeStation[timeStation.size()-1]);
		Individual* mutante = new Individual(population[ind]->getModels(), wf, task, newStations,timeStation,wo);
		population.push_back(mutante);
		return true;
	}
	return true;
}

/*
 * Gera vetor de estações atualizado apos a mutação de tarefas
 */
vector<int> GeneticAlgorithm::updateStationMutation(int knew, int kold, int ind, vector<int> stationsAux){
	int last, earl;
	vector<int> stations = stationsAux;
	bool special=false;
	bool newK = false;
	if (knew < kold) {
		last = lastPositionStation(ind, knew);
		earl = earlyPositionStation(ind, kold);

		stations[(last+1)] = knew;
		stations[(earl-1)] = kold-1;
		if ((kold-knew) > 3 ) {
			special = true;
		}
	}else{
		last = lastPositionStation(ind, kold);
		earl = earlyPositionStation(ind, knew);
		if (earl == -1) {
			earl = stations.size();
			newK = true;
		}
		stations[(last)] = kold+1;
		stations[(earl-1)] = knew;

		if ((knew-kold) > 3 ) {
			special=true;
		}
	}

	if (special) {
		for (int i = last+1; i <= earl-1; i++) {
			stations[i+1] = stationsAux[i];
		}
	}
	return stations;
}

/*
 * Realiza crossover
 */
void GeneticAlgorithm::crossover(int porc){
	int qtdF = (sizePop*porc)/100;
	int i =0;
	set<int> paisS;
	int p1=-1;
	int p2=-1;
	infacCrossover =0;
	
	while(i < qtdF){
		int tamAnt = paisS.size()+1;
		p1 = rand() % population.size();
		while(paisS.size()!=tamAnt){
			if (paisS.find(p1) == paisS.end()) {
				paisS.insert(p1);
			}else{
				p1++;
				if (p1 >= population.size()) {
					p1=0;
				}
			}
		}
		tamAnt = paisS.size()+1;
		p2 = rand() % population.size();
		while(paisS.size()!=tamAnt){
			if (paisS.find(p2) == paisS.end()) {
				paisS.insert(p2);
			}else{
				p2++;
				if (p2 >= population.size()) {
					p2=0;
				}
			}
		}

		if (generateF(p1,p2)) {
			i++;
		}else{
			i++;
			infacCrossover++;
			paisS.erase(p1);
			paisS.erase(p2);
		}
		p1=-1;
		p2=-1;
	}
}

/*
 * Gera filhos do p1 com p2
 */
bool GeneticAlgorithm::generateF(int p1, int p2){

	// Filho 1

	vector<int> tf = generateFTasks(population[p1]->getTasks(),population[p2]->getTasks());
	vector<int> sf = generateIndividualStation(tf);
	vector<int> wf = generateIndividualWorker(tf, sf);
	bool f1ok = false;
	if (wf != nulo){
		f1ok=true;
		vector<int> mf = generateFModels(population[p1]->getModels(),population[p2]->getModels());
		vector<double> timeStation = generateTimeStation(tf, wf,sf );
		double wo = generateWo(tf, wf, sf, mf , timeStation[timeStation.size()-1]);
		Individual* f1 = new Individual(mf, wf, tf, sf,timeStation,wo);
		population.push_back(f1);
	}

	// Filho 2

	tf = generateFTasks(population[p2]->getTasks(),population[p1]->getTasks());
	sf = generateIndividualStation(tf);
	wf = generateIndividualWorker(tf, sf);
	bool f2ok = false;
	if (wf != nulo){
		f2ok=true;
		vector<int> mf = generateFModels(population[p2]->getModels(),population[p1]->getModels());

		vector<double> timeStation = generateTimeStation(tf, wf,sf);
		double wo = generateWo(tf, wf, sf, mf , timeStation[timeStation.size()-1]);
		Individual* f2 = new Individual(mf, wf, tf, sf,timeStation,wo);
		population.push_back(f2);
	}

	if (!f1ok && !f2ok) {
		//FILHOS INFACTIVEIS
		wf = generateFWorkerInfeasible();
		vector<int> mf = generateFModels(population[p2]->getModels(),population[p1]->getModels());
		vector<double> timeStation = population[p1]->getTimeStation();
		double wo = population[p1]->getWo();
		Individual* f = new Individual(mf, wf, tf, sf,timeStation,wo);
		f->isInfeasible(qtdInfacGenes, assemblyLine->getK());
		qtdInfacGenes=0;
		population.push_back(f);
		return false;
	}
	return true;
}

/*
 * Gera vetor de trabalhadores é ordem crescente para individuos infactiveis
 */
vector<int> GeneticAlgorithm::generateFWorkerInfeasible(){
	vector<int> w;
	w.resize(assemblyLine->getK());
	for (int i = 0; i < assemblyLine->getK(); i++) {
		w[i] = i;
	}
	return w;
}

/*
 * Gera vetor de tarefas do p1 e p2
 */
vector<int> GeneticAlgorithm::generateFTasks(vector<int> p1,vector<int> p2){

	int pmeio = p1.size()/2;
	set<int> tasksS;
	vector<int> f;
	f.resize(p1.size());

	for (int i = 0; i < pmeio; i++) {
		f[i]= p1[i];
		tasksS.insert(p1[i]);
	}
	int i = pmeio;
	int j = 0;
	while(i < p1.size()){
		if (tasksS.find(p2[j]) == tasksS.end()) {
			f[i] = p2[j];
			i++;
		}
		j++;
	}
	return f;
}

 /*
  * Gera um individuo
  */
Individual* GeneticAlgorithm::generateIndividual(){

	vector<int> modelI = generateIndividualModel();
	vector<int> tasksI = generateIndividualTasks();
	vector<int> stationI = generateIndividualStation(tasksI);
	vector<int> workerI = generateIndividualWorker(tasksI, stationI);
	if (workerI == nulo){
		workerI = generateFWorkerInfeasible();
		vector<double> timeStation = generateTimeStation(tasksI, workerI,stationI );
		double wo = generateWo(tasksI, workerI, stationI, modelI , timeStation[timeStation.size()-1]);
		Individual* b =new Individual(modelI, workerI, tasksI, stationI , timeStation,wo);
		b->isInfeasible(assemblyLine->getK(),assemblyLine->getK());
		return b;
	} else{
		vector<double> timeStation = generateTimeStation(tasksI, workerI,stationI );
		double wo = generateWo(tasksI, workerI, stationI, modelI , timeStation[timeStation.size()-1]);

		for (int i = 0; i < timeStation.size(); i++) {
			cerr << timeStation[i] << " -- ";
		}
		return new Individual(modelI, workerI, tasksI, stationI , timeStation,wo);
	}
}

/*
 * Gera vetor com tempo das estações
 */
vector<double> GeneticAlgorithm::generateTimeStation(vector<int> tasks, vector<int> workers, vector <int> station ){
	// Variaveis auxiliares para verificar quais tarefas de cada estaçao

	int t_inicio=0;
	int t_limite = assemblyLine->getN();
	// Vetor com o tempo de cada estaçao, onde a ultima posiçao é o tempo da estação mais pesada, ou seja, o tempo de ciclo
	vector<double> timeStation;
	timeStation.resize(assemblyLine->getK()+1);

	// Variavel para guarda o maior tempo das estações
	double somaMaior = 0;

	for (int k = 0; k < assemblyLine->getK(); k++) {
		// Variavel para guarda o tempo da estação
		double somaT =0;
		if (t_limite != assemblyLine->getN()) {
			t_inicio = t_limite;
			t_limite = assemblyLine->getN();
		}
		for (int m = 0; m < assemblyLine->getM(); m++) {
			double soma =0;
			for (int n = t_inicio; n < t_limite; n++) {
				// Verifica se o vetor de estações está na estação k
				if (station[n] != k) {
					t_limite = n;
					break;
				}else{
					soma+= allTask[tasks[n]]->getExecutionTime(workers[k],m);
				}
			}
			somaT += soma*assemblyLine->getModel(m)->getFractionDemand();

		}
		
		if (somaMaior < somaT) {
			somaMaior = somaT;
		}
		timeStation[k] = somaT;
	}

	timeStation[assemblyLine->getK()] = somaMaior;

	for (int i = 0; i < timeStation.size(); i++) {
		cerr << timeStation[i] << ",";
	}
	return timeStation;
}

/*
 * Gera wo
 */
double GeneticAlgorithm::generateWo(vector<int> tasks, vector<int> workers, vector <int> station, vector<int> models , double t_cycle){

	int t_inicio=0;
	int t_limite = assemblyLine->getN();
	vector<Station> stationsTemp;

	for (int k = 0; k < assemblyLine->getK(); k++) {
		Station temp(k);
		if (t_limite != assemblyLine->getN()) {
			t_inicio = t_limite;
			t_limite = assemblyLine->getN();
		}

		for (int n = t_inicio; n < t_limite; n++) {

			if (station[n] != k) {
				t_limite = n;
				break;
			}else{
				temp.addTask(allTask[tasks[n]]);

			}

		}
		Worker w(workers[k]);
		temp.setAssignedWorker(w);
		temp.calculateTmk(assemblyLine->getM(), assemblyLine->getW());
		temp.calculateWOmk(assemblyLine->getM(), t_cycle, t_cycle*1.01);
		stationsTemp.push_back(temp);
	}

	double total = 0;

	for (int i = 0; i < models.size(); i++) {
		total += getApproximateWo(i, models,stationsTemp, models.size());
	}
	return total;
}

/*
 * Retorna o WO aproximado : Feito por Pamela adaptado para o GA
 */
double GeneticAlgorithm::getApproximateWo(int i, vector<int> xh, vector<Station> stationsTemp, int I) {
    int m, m1, k;
    double aux = 0, aux2;
    int K = assemblyLine->getK();

    if (i >=0 && i < I) {
        m = xh[i];
        m1 = i+1 < I ? xh[i+1] : -1;

        for (k = 0; k < K; k++) {
            if (stationsTemp[k].getWOmk(m) > 0 && stationsTemp[k].getDelta(m) > 0) {
                if (m1 >= 0 && stationsTemp[k].getWOmk(m1) < 0 && stationsTemp[k].getDelta(m1) > 0) {
                    aux2 = stationsTemp[k].getWOmk(m)/stationsTemp[k].getDelta(m) + stationsTemp[k].getWOmk(m1)/stationsTemp[k].getDelta(m1);
                    aux += aux2 > 0 ? aux2 : 0;
                } else {
                    aux += stationsTemp[k].getWOmk(m)/stationsTemp[k].getDelta(m);
                }
            }
        }
    }

    return aux;
}

/*
 * Gera vetor de modelos aleatorio
 */
vector<int> GeneticAlgorithm::generateIndividualModel(){

	int totalDemand = this->assemblyLine->getTotalDemandAvg();
	int qtdModels = this->assemblyLine->getM();

	vector<int> v(totalDemand);

	for (int m = 0; m < qtdModels; m++) {

		Model *model = this->assemblyLine->getModel(m);
		int demand = model->getDemand();
		int soma = 0;
		while (soma<demand) {
			int indice = rand() % totalDemand;
			if (v[indice]==0) {
				v[indice] = m;
				soma++;
			}
		}

	}

	return v;

}

/*
 * Gera vetor de trabalhadores baseado nas tarefas e estações
 */
vector<int> GeneticAlgorithm::generateIndividualWorker(vector<int> tasksI, vector<int> stationI){

	vector< vector<int> > infactS;
	infactS.resize(assemblyLine->getK());

	for (int p = 0; p < assemblyLine->getK(); p++)
		infactS[p].resize(assemblyLine->getK() + 1, 0);

	for (int w = 0; w < assemblyLine->getK(); w++) {

		for (int s = 0; s < tasksI.size() ; s++) {

			Task* t = this->allTask[tasksI[s]];

			if (t->getAvgExecutionTime()[w] > MAX-10){

				if (infactS[w][stationI[s]] == 0) {
					infactS[w][stationI[s]] = -1;
					infactS[w][assemblyLine->getK()] = infactS[w][assemblyLine->getK()]+1;
				}

				if (infactS[w][assemblyLine->getK()] == assemblyLine->getK()) {
					this->qtdInfacGenes = assemblyLine->getK();
					return nulo;
				}
			}
		}
	}
	vector<int> workerI;
	workerI.resize(assemblyLine->getK(), -1);
	for (int s = 0; s < assemblyLine->getK(); s++) {
		vector<int> sort;
		for (int w = 0; w < assemblyLine->getK(); w++) {
			if (!containsContent(workerI,w) && infactS[w][s] != -1) {
				if (sort.size() == 0 ) {
					sort.push_back(w);
				}else if(sort.size()>0){
					int ultimo = sort[sort.size()-1];
					if (infactS[ultimo][assemblyLine->getK()] <  infactS[w][assemblyLine->getK()]) {
						sort[sort.size()-1] = w;
					}else if(infactS[ultimo][assemblyLine->getK()] ==  infactS[w][assemblyLine->getK()]){
						sort.push_back(w);
					}
				}
			}
		}

		if (sort.size() > 0) {
			int r = rand()%sort.size();
			workerI[s] = sort[r];
		}else{
			qtdInfacGenes = assemblyLine->getK() -s;
			return nulo;
		}
	}
	return workerI;
}

/*
 * Gera vetor de estações baseado no tempo de ciclo e nas estações
 */
vector<int> GeneticAlgorithm::generateIndividualStation(vector<int> taskI){

	vector<int> stationI;
	stationI.resize(assemblyLine->getN(), -1);
	int taskAtual=taskI[0];
	int n=0;
	vector<int> timeStation;
	timeStation.resize(assemblyLine->getK(), 0);

	for (int k = 0; k < assemblyLine->getK(); k++) {
		if (k!=(assemblyLine->getK()-1)) {

			while((timeStation[k] + allTask[taskAtual]->getAvgExecutionTime(assemblyLine->getK(), assemblyLine->getM())) < cycleTime){
				stationI[n] = k;
				timeStation[k] += allTask[taskAtual]->getAvgExecutionTime(assemblyLine->getK(), assemblyLine->getM());
				n++;
				if (n >= taskI.size()) {
					return stationI;
				}
				taskAtual = taskI[n];
			}
		}else{
			for (int j = n;j < assemblyLine->getN(); j++) {
				taskAtual = taskI[j];
				stationI[j] = k;
				timeStation[k] += allTask[taskAtual]->getAvgExecutionTime(assemblyLine->getK(), assemblyLine->getM());
			}
		}
	}
	timeStationTemp = timeStation;
	return stationI;
}

/*
 * Gera vetor aleatorio de tarefas
 */
vector<int> GeneticAlgorithm::generateIndividualTasks(){
	vector< vector<int> > tableTasksTemp;
	tableTasksTemp.resize(assemblyLine->getN());
	for (int p = 0; p < assemblyLine->getN(); p++)
		tableTasksTemp[p].resize(assemblyLine->getN());

	tableTasksTemp = tableTasks;
	vector<int> taskI;
	taskI.resize(assemblyLine->getN(), -1);

	for (int p = 0; p < assemblyLine->getN(); p++) {
		vector<int> listSort = tasksFree(tableTasksTemp, taskI);
		if (listSort.size() == 0) {
			break;
		}
		int v = rand()%listSort.size();
		taskI[p] = listSort[v];
		for (int f = 0; f < assemblyLine->getN(); f++) {
			tableTasksTemp[listSort[v]][f] =0;
		}

	}

	return taskI;
}

/*
 * Retorna um vetor com as tarefas que estão livres para ser inseridas no vetor de tarefas
 */
vector<int> GeneticAlgorithm::tasksFree(vector< vector<int> > tableTasksTemp, vector<int> taskI){

	vector<int> tF;
	for (int c = 0; c < assemblyLine->getN(); c++) {

		if (!containsContent(taskI, c)) {
			int somaTemp=0;
			for (int l = 0; l < assemblyLine->getN(); l++) {
				somaTemp = somaTemp + tableTasksTemp[l][c];
			}
			if (somaTemp == 0) {
				tF.push_back(c);
			}
		}

	}

	return tF;
}

/*
 * Imprime vetor
 */
void GeneticAlgorithm::printVector(vector<int> v){
	for (int i = 0; i < v.size(); i++) {
		cerr << v[i] << ",";
	}
}

/*
 * Imprimi melhores os melhores
 */
void GeneticAlgorithm::printTheBest(){

	double bestFitness = MAX;
	double bestCycleTime = MAX;
	double bestWo = MAX;
	double avgFitness = 0;
	double avgCycleTime = 0;
	double avgWo = 0;

	for (int i = 0; i < theBest.size(); i++) {

		cout << "[" << i << "] " ;
		theBest[i]->printFitness();
		avgFitness += theBest[i]->getFitness();
		avgCycleTime += theBest[i]->getCycleTime();
		avgWo += theBest[i]->getWo();

		if (bestCycleTime > theBest[i]->getCycleTime()) {
			bestCycleTime = theBest[i]->getCycleTime();
		}

		if (bestWo > theBest[i]->getWo()) {
			bestWo = theBest[i]->getWo();
		}

		if (bestFitness > theBest[i]->getFitness()) {

			bestFitness = theBest[i]->getFitness();
		}

	}

	avgFitness = avgFitness/theBest.size();
	avgCycleTime = avgCycleTime/theBest.size();
	avgWo = avgWo/theBest.size();

	cout << " Best Fitness : " << bestFitness << ": Best Wo : " << bestWo << ": Best CycleTime : " << bestCycleTime  << endl;
	theBest[theBest.size()-1]->printAll();
}

void GeneticAlgorithm::printPopulation(){
	double bestFitness = MAX;
	double bestCycleTime = MAX;
	double bestWo = MAX;
	double avgFitness = 0;
	double avgCycleTime = 0;
	double avgWo = 0;
	int pOfBestFitness=0;
	for (int i = 0; i < population.size(); i++) {

		avgFitness += population[i]->getFitness();
		avgCycleTime += population[i]->getCycleTime();
		avgWo += population[i]->getWo();

		if (bestCycleTime > population[i]->getCycleTime()) {
			bestCycleTime = population[i]->getCycleTime();
		}

		if (bestWo > population[i]->getWo()) {
			bestWo = population[i]->getWo();
		}

		if (bestFitness > population[i]->getFitness()) {
			pOfBestFitness = i;
			bestFitness = population[i]->getFitness();
		}
	}

	theBest.push_back(population[pOfBestFitness]);
	fitnessTheBest = population[pOfBestFitness]->getFitness();
	avgFitness = avgFitness/population.size();
	avgCycleTime = avgCycleTime/population.size();
	avgWo = avgWo/population.size();
	cerr << "Media_fitnes: " << avgFitness << " Media_ciclo: " << avgCycleTime 
		<< " Media_wo: " << avgWo << " Melhor_fitnes: " << population[pOfBestFitness]->getFitness()
		<<" Melhor_ciclo: " << population[pOfBestFitness]->getCycleTime()
		<<" Melhor_wo: " << population[pOfBestFitness]->getWo()<< endl;
}

void GeneticAlgorithm::selection(){
	qtdInfacNewPop =0;
	cycleTime = cycleTime*1.001;
	double fitTotal =0;
	map<double, int> fit_posicao;
	set<double> fit_ordenado;
	map<double, double> fitposicao_fitordenado;

	for (int i = 0; i < population.size(); i++) {
		fitTotal += population[i]->getFitness();
		fit_ordenado.insert(population[i]->getFitness());
		fit_posicao[population[i]->getFitness()] = i;
	}
	double xTemp = 0;

	for (set<double>::reverse_iterator rit=fit_ordenado.rbegin(); rit != fit_ordenado.rend(); ++rit){
		xTemp += *rit;
		fitposicao_fitordenado[(*rit/fitTotal)] = *rit;
	}

	vector<Individual*> popAux;
	popAux.resize(sizePop);

	for (int p = 0; p < sizePop; p++) {
		double randomico = rand()%1;
		double epsilon = 1.0001;
		map<double,double>::iterator it = fitposicao_fitordenado.upper_bound(randomico*epsilon);
		int posicao = fit_posicao[(it->second)];
		popAux[p]=(population[posicao]);

		if(it->second>1) fitposicao_fitordenado.erase (it);
	}

	population.clear();
	population.resize(sizePop);
	population = popAux;
}

vector<Individual*> GeneticAlgorithm::getTheBest(){
	return theBest;
}
void GeneticAlgorithm::printDemandFrac(){
	for (int i = 0; i < assemblyLine->getM(); i++) {
		cerr << "Demanda fracionada de " << i << " : " << assemblyLine->getModel(i)->getFractionDemand() << endl;
	}
}
void GeneticAlgorithm::generateNewIndividuals(){
	Individual* i = generateIndividual();
	if (i != nullo) {
		population.push_back(i);
	}
}