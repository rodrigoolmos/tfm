import tkinter as tk
from tkinter import ttk

def submit():
    # Obtener los valores ingresados
    number = entry_number.get()
    sex = sex_var.get()
    red_pixel = entry_red_pixel.get()
    green_pixel = entry_green_pixel.get()
    blue_pixel = entry_blue_pixel.get()
    haemoglobin_level = entry_haemoglobin_level.get()
    anaemic = anaemic_var.get()
    
    pregnancies = entry_pregnancies.get()
    glucose = entry_glucose.get()
    blood_pressure = entry_blood_pressure.get()
    skin_thickness = entry_skin_thickness.get()
    insulin = entry_insulin.get()
    bmi = entry_bmi.get()
    diabetes_pedigree = entry_diabetes_pedigree.get()
    age = entry_age.get()
    outcome = outcome_var.get()
    
    person_age = entry_person_age.get()
    person_gender = person_gender_var.get()
    chest_pain = chest_pain_var.get()
    resting_bp = entry_resting_bp.get()
    cholesterol = entry_cholesterol.get()
    fasting_bs = fasting_bs_var.get()
    resting_ecg = resting_ecg_var.get()
    max_heart_rate = entry_max_heart_rate.get()
    exercise_angina = exercise_angina_var.get()
    previous_peak = entry_previous_peak.get()
    slope = slope_var.get()
    major_vessels = entry_major_vessels.get()
    thal_rate = entry_thal_rate.get()
    
    individual_gender = individual_gender_var.get()
    individual_age = entry_individual_age.get()
    smoking = smoking_var.get()
    yellow_fingers = yellow_fingers_var.get()
    anxiety = anxiety_var.get()
    peer_pressure = peer_pressure_var.get()
    chronic_disease = chronic_disease_var.get()
    fatigue = fatigue_var.get()
    allergy = allergy_var.get()
    wheezing = wheezing_var.get()
    alcohol_consuming = alcohol_consuming_var.get()
    coughing = coughing_var.get()
    shortness_of_breath = shortness_of_breath_var.get()
    swallowing_difficulty = swallowing_difficulty_var.get()
    chest_pain_new = chest_pain_new_var.get()
    
    # Imprimir los datos agrupados por secciones
    print("Datos Generales:")
    print(f"Number: {number}")
    print(f"Sex: {sex}")
    print(f"Red Pixel: {red_pixel}")
    print(f"Green Pixel: {green_pixel}")
    print(f"Blue Pixel: {blue_pixel}")
    print(f"Haemoglobin Level: {haemoglobin_level}")
    print(f"Anaemic: {anaemic}")
    
    print("\nDatos de Salud:")
    print(f"Pregnancies: {pregnancies}")
    print(f"Glucose: {glucose}")
    print(f"Blood Pressure: {blood_pressure}")
    print(f"Skin Thickness: {skin_thickness}")
    print(f"Insulin: {insulin}")
    print(f"BMI: {bmi}")
    print(f"Diabetes Pedigree Function: {diabetes_pedigree}")
    print(f"Age: {age}")
    print(f"Outcome: {outcome}")
    
    print("\nDatos Cardíacos:")
    print(f"Age of the person: {person_age}")
    print(f"Gender of the person: {person_gender}")
    print(f"Chest Pain type: {chest_pain}")
    print(f"Resting blood pressure: {resting_bp}")
    print(f"Cholesterol: {cholesterol}")
    print(f"Fasting blood sugar > 120 mg/dl: {fasting_bs}")
    print(f"Resting electrocardiographic results: {resting_ecg}")
    print(f"Maximum heart rate achieved: {max_heart_rate}")
    print(f"Exercise induced angina: {exercise_angina}")
    print(f"Previous peak: {previous_peak}")
    print(f"Slope: {slope}")
    print(f"Number of major vessels (0-3): {major_vessels}")
    print(f"Thal rate: {thal_rate}")
    
    print("\nHábitos y Síntomas:")
    print(f"Gender of the individual: {individual_gender}")
    print(f"Age of the individual: {individual_age}")
    print(f"Smoking: {smoking}")
    print(f"Yellow Fingers: {yellow_fingers}")
    print(f"Anxiety: {anxiety}")
    print(f"Peer Pressure: {peer_pressure}")
    print(f"Chronic Disease: {chronic_disease}")
    print(f"Fatigue: {fatigue}")
    print(f"Allergy: {allergy}")
    print(f"Wheezing: {wheezing}")
    print(f"Alcohol Consuming: {alcohol_consuming}")
    print(f"Coughing: {coughing}")
    print(f"Shortness of Breath: {shortness_of_breath}")
    print(f"Swallowing Difficulty: {swallowing_difficulty}")
    print(f"Chest Pain: {chest_pain_new}")

# Crear la ventana principal
root = tk.Tk()
root.title("Formulario de Parámetros")

# Crear el contenedor principal
main_frame = ttk.Frame(root)
main_frame.pack(pady=10, expand=True, fill='both')

# Crear un contenedor para cada sección
frame_generales = ttk.LabelFrame(main_frame, text="Datos Generales")
frame_generales.grid(row=0, column=0, padx=10, pady=5, sticky="n")

frame_salud = ttk.LabelFrame(main_frame, text="Datos de Salud")
frame_salud.grid(row=0, column=1, padx=10, pady=5, sticky="n")

frame_cardiacos = ttk.LabelFrame(main_frame, text="Datos Cardíacos")
frame_cardiacos.grid(row=0, column=2, padx=10, pady=5, sticky="n")

frame_habitos = ttk.LabelFrame(main_frame, text="Hábitos y Síntomas")
frame_habitos.grid(row=0, column=3, padx=10, pady=5, sticky="n")

# Variables
sex_var = tk.StringVar()
anaemic_var = tk.StringVar()
outcome_var = tk.StringVar()
person_gender_var = tk.StringVar()
chest_pain_var = tk.StringVar()
fasting_bs_var = tk.StringVar()
resting_ecg_var = tk.StringVar()
exercise_angina_var = tk.StringVar()
slope_var = tk.StringVar()
individual_gender_var = tk.StringVar()
smoking_var = tk.StringVar()
yellow_fingers_var = tk.StringVar()
anxiety_var = tk.StringVar()
peer_pressure_var = tk.StringVar()
chronic_disease_var = tk.StringVar()
fatigue_var = tk.StringVar()
allergy_var = tk.StringVar()
wheezing_var = tk.StringVar()
alcohol_consuming_var = tk.StringVar()
coughing_var = tk.StringVar()
shortness_of_breath_var = tk.StringVar()
swallowing_difficulty_var = tk.StringVar()
chest_pain_new_var = tk.StringVar()

# Datos Generales
tk.Label(frame_generales, text="Number").grid(row=0, column=0, padx=10, pady=5)
entry_number = tk.Entry(frame_generales)
entry_number.grid(row=0, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Sex").grid(row=1, column=0, padx=10, pady=5)
ttk.Combobox(frame_generales, textvariable=sex_var, values=["Male", "Female"]).grid(row=1, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Red Pixel").grid(row=2, column=0, padx=10, pady=5)
entry_red_pixel = tk.Entry(frame_generales)
entry_red_pixel.grid(row=2, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Green Pixel").grid(row=3, column=0, padx=10, pady=5)
entry_green_pixel = tk.Entry(frame_generales)
entry_green_pixel.grid(row=3, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Blue Pixel").grid(row=4, column=0, padx=10, pady=5)
entry_blue_pixel = tk.Entry(frame_generales)
entry_blue_pixel.grid(row=4, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Haemoglobin Level").grid(row=5, column=0, padx=10, pady=5)
entry_haemoglobin_level = tk.Entry(frame_generales)
entry_haemoglobin_level.grid(row=5, column=1, padx=10, pady=5)

tk.Label(frame_generales, text="Anaemic").grid(row=6, column=0, padx=10, pady=5)
ttk.Combobox(frame_generales, textvariable=anaemic_var, values=["Yes", "No"]).grid(row=6, column=1, padx=10, pady=5)

# Datos de Salud
tk.Label(frame_salud, text="Pregnancies").grid(row=0, column=0, padx=10, pady=5)
entry_pregnancies = tk.Entry(frame_salud)
entry_pregnancies.grid(row=0, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Glucose").grid(row=1, column=0, padx=10, pady=5)
entry_glucose = tk.Entry(frame_salud)
entry_glucose.grid(row=1, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Blood Pressure").grid(row=2, column=0, padx=10, pady=5)
entry_blood_pressure = tk.Entry(frame_salud)
entry_blood_pressure.grid(row=2, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Skin Thickness").grid(row=3, column=0, padx=10, pady=5)
entry_skin_thickness = tk.Entry(frame_salud)
entry_skin_thickness.grid(row=3, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Insulin").grid(row=4, column=0, padx=10, pady=5)
entry_insulin = tk.Entry(frame_salud)
entry_insulin.grid(row=4, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Body Mass Index (BMI)").grid(row=5, column=0, padx=10, pady=5)
entry_bmi = tk.Entry(frame_salud)
entry_bmi.grid(row=5, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Diabetes Pedigree Function").grid(row=6, column=0, padx=10, pady=5)
entry_diabetes_pedigree = tk.Entry(frame_salud)
entry_diabetes_pedigree.grid(row=6, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Age").grid(row=7, column=0, padx=10, pady=5)
entry_age = tk.Entry(frame_salud)
entry_age.grid(row=7, column=1, padx=10, pady=5)

tk.Label(frame_salud, text="Outcome").grid(row=8, column=0, padx=10, pady=5)
ttk.Combobox(frame_salud, textvariable=outcome_var, values=["Positive", "Negative"]).grid(row=8, column=1, padx=10, pady=5)

# Datos Cardíacos
tk.Label(frame_cardiacos, text="Age of the person").grid(row=0, column=0, padx=10, pady=5)
entry_person_age = tk.Entry(frame_cardiacos)
entry_person_age.grid(row=0, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Gender of the person").grid(row=1, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=person_gender_var, values=["Male", "Female"]).grid(row=1, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Chest Pain type").grid(row=2, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=chest_pain_var, values=["Type 1", "Type 2", "Type 3", "Type 4"]).grid(row=2, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Resting blood pressure").grid(row=3, column=0, padx=10, pady=5)
entry_resting_bp = tk.Entry(frame_cardiacos)
entry_resting_bp.grid(row=3, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Cholesterol").grid(row=4, column=0, padx=10, pady=5)
entry_cholesterol = tk.Entry(frame_cardiacos)
entry_cholesterol.grid(row=4, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Fasting blood sugar > 120 mg/dl").grid(row=5, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=fasting_bs_var, values=["True", "False"]).grid(row=5, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Resting electrocardiographic results").grid(row=6, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=resting_ecg_var, values=["Result 1", "Result 2", "Result 3"]).grid(row=6, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Maximum heart rate achieved").grid(row=7, column=0, padx=10, pady=5)
entry_max_heart_rate = tk.Entry(frame_cardiacos)
entry_max_heart_rate.grid(row=7, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Exercise induced angina").grid(row=8, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=exercise_angina_var, values=["Yes", "No"]).grid(row=8, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Previous peak").grid(row=9, column=0, padx=10, pady=5)
entry_previous_peak = tk.Entry(frame_cardiacos)
entry_previous_peak.grid(row=9, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Slope").grid(row=10, column=0, padx=10, pady=5)
ttk.Combobox(frame_cardiacos, textvariable=slope_var, values=["Slope 1", "Slope 2", "Slope 3"]).grid(row=10, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Number of major vessels (0-3)").grid(row=11, column=0, padx=10, pady=5)
entry_major_vessels = tk.Entry(frame_cardiacos)
entry_major_vessels.grid(row=11, column=1, padx=10, pady=5)

tk.Label(frame_cardiacos, text="Thal rate").grid(row=12, column=0, padx=10, pady=5)
entry_thal_rate = tk.Entry(frame_cardiacos)
entry_thal_rate.grid(row=12, column=1, padx=10, pady=5)

# Hábitos y Síntomas
tk.Label(frame_habitos, text="Gender of the individual").grid(row=0, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=individual_gender_var, values=["M", "F"]).grid(row=0, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="Age of the individual").grid(row=1, column=0, padx=10, pady=5)
entry_individual_age = tk.Entry(frame_habitos)
entry_individual_age.grid(row=1, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="SMOKING").grid(row=2, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=smoking_var, values=["Yes", "No"]).grid(row=2, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="YELLOW FINGERS").grid(row=3, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=yellow_fingers_var, values=["Yes", "No"]).grid(row=3, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="ANXIETY").grid(row=4, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=anxiety_var, values=["Yes", "No"]).grid(row=4, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="PEER PRESSURE").grid(row=5, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=peer_pressure_var, values=["Yes", "No"]).grid(row=5, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="CHRONIC DISEASE").grid(row=6, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=chronic_disease_var, values=["Yes", "No"]).grid(row=6, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="FATIGUE").grid(row=7, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=fatigue_var, values=["Yes", "No"]).grid(row=7, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="ALLERGY").grid(row=8, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=allergy_var, values=["Yes", "No"]).grid(row=8, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="WHEEZING").grid(row=9, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=wheezing_var, values=["Yes", "No"]).grid(row=9, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="ALCOHOL CONSUMING").grid(row=10, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=alcohol_consuming_var, values=["Yes", "No"]).grid(row=10, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="COUGHING").grid(row=11, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=coughing_var, values=["Yes", "No"]).grid(row=11, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="SHORTNESS OF BREATH").grid(row=12, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=shortness_of_breath_var, values=["Yes", "No"]).grid(row=12, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="SWALLOWING DIFFICULTY").grid(row=13, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=swallowing_difficulty_var, values=["Yes", "No"]).grid(row=13, column=1, padx=10, pady=5)

tk.Label(frame_habitos, text="CHEST PAIN").grid(row=14, column=0, padx=10, pady=5)
ttk.Combobox(frame_habitos, textvariable=chest_pain_new_var, values=["Yes", "No"]).grid(row=14, column=1, padx=10, pady=5)

# Botón de submit
tk.Button(root, text="Submit", command=submit).pack(pady=10)

# Ejecutar la aplicación
root.mainloop()
