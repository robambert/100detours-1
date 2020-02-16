""" Helper to generate random data
"""

############################################### PART TO FETCH ADRESSES
import pandas as pd
import numpy as np
data = pd.read_csv('adresse_paris.csv', sep=';')
needed_columns = ['Geometry X Y','N_VOIE', 'C_AR','L_ADR']
df = data[needed_columns]


def fetch_adress(serie):
    serie=serie
    arr = '750'+str(int(serie['C_AR'])) if int(serie['C_AR'])>10 else '7500'+str(int(serie['C_AR']))
    return f" {serie['L_ADR']} {arr} Paris"

def get_adresses(arrondissements = [1,2,3,4,5,6,7,8,9,10], n=260, df = df):
    results = {arr : [] for arr in arrondissements}
    n_per_arr = n//len(arrondissements)
    for arr in arrondissements:
        arr_adresses = df[df['C_AR'] == arr].sample(n_per_arr)
        results[arr] = [{'adress' : fetch_adress(adress[1]), 'x' :adress[1]['Geometry X Y'].split(',')[0],
                         'y' : adress[1]['Geometry X Y'].split(',')[1] }
                        for adress in arr_adresses.iterrows()]
    return results

    
    
############################################### CREATE NURSES AND PATIENTS
    
    
noms = """Thibaud Deloffre
Jean-Christophe Plessis
Séverin Barreau
Yves Dujardin
Nathan Trottier
Raymond Pinchon
Samuel Cerf
Hubert Cahun
Eugène Pasquier
Théo Chéron
Julia Schaeffer
Capucine Ancel
Marie-Thérèse Jacquard
Rolande Pierrat
Solène Simon
Rébecca Ballesdens
Rose-Marie Sardou
Alizée Bachelot
Ambre Charbonneau
Lise Pomeroy
Maël Matthieu
Armand Devereaux
Léon Brunet
Matthias Génin
Noé Bourgeois
Martin Pueyrredón
Jérôme Beauvau
Florent D'Amboise
Jean Philippon
Thibaud Vidal
Émilienne Hachette
Radegonde Bozonnet
Caroline Leroux
Clarisse Bourgeois
Sarah Lémery
Sylvie Beaux
Léa Auch
Gabrielle Deloffre
Mélanie Loupe
Romane Droz
Jérémie Matthieu
Élisée Fouché
Émilien Carrel
Max Carrell
Aubin Loupe
Martial Vannier
José Laffitte
Richard Vandame
Noël Ardouin
Marcel Geffroy
Zoé Bourguignon
Léonie Geffroy
Gwenaëlle Fresnel
Nina Baumé
Marlène Gueguen
Constance Flandin
Marielle Berger
Clotilde Jullien
Victoire Trémaux
Laura Courbis
Abeau Bain
Richard Lussier
Matthieu Simon
Zacharie Bourdon
Ludovic Maret
Blaise Cerfbeer
Serge Crozier
Claude LeBeau
Georges Trémaux
Pierre-Marie Joguet
Laëtitia Carrel
Inès Bardin
Charlène Calvet
Barbe Vaganay
Gwenaëlle Chopin
Edwige Besnard
Godeliève Rodier
Bérengère Bouthillier
Justine Houdin
Clémence Pomeroy
Ange Bruguière
Adolphe Delaunay
Mickaël Charpentier
Jean Portier
Noël Dieudonné
Timothé Balzac
François Tremblay
Christopher Périer
Kévin Bourgeois
Abel Auguste
"""
nomss = noms.split('\n')[:-1]
def create_patient(adresses,noms):
    choice_arr = np.random.randint(1,len(adresses))
    choice_addr = np.random.randint(0, len(adresses[choice_arr]))
    choice = adresses[choice_arr][choice_addr]
    patient = {'name' : str(np.random.choice(noms)),
               'adress' : choice}

    return patient


def create_nurse(user, adresses, noms):
    choice_arr = np.random.randint(1,len(adresses))
    choice_addr = np.random.randint(0, len(adresses[choice_arr]))
    choice = adresses[choice_arr][choice_addr]
    nb_treatmtents_forbidden = np.random.randint(3)
    treatment_types = np.random.choice(np.arange(11), nb_treatmtents_forbidden)  
    name = str(np.random.choice(noms))
    start_hour = np.random.randint(8,13)
    start_hour = f'0{start_hour}' if start_hour<10 else start_hour
    agenda = []
    for i in range(6,10):
        agenda.append(f'2020-01-0{i}T{start_hour}:00:00')
    nurse = {'user' : user,
             'name' : name,
             'treatment_types' : list(treatment_types),
             'agenda' : agenda,
             'adress' : choice}
    return nurse

adresses = get_adresses(n=2000, df = pd.read_csv('adresse_paris.csv', sep=';'))
patients = []
nurses = []
nb_patients = 240
nb_nurses = 20
for i in range(nb_patients):
    patients.append(create_patient(adresses, nomss))
for i in range(nb_nurses):
    nurses.append(create_nurse(i+1, adresses, nomss))

def convert(o):
    if isinstance(o, np.int32): return int(o)  
    raise TypeError    
with open('patients.json','w') as f:
    json.dump(patients, f, default=convert)

    
