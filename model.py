from sklearn.ensemble import RandomForestRegressor
import pandas as pd

# Load dataset
dataset_filename = "dataset.csv"
df = pd.read_csv(dataset_filename)

# Features and target variable
X = df[['sampah_per_jam', 'ketinggian_sampah', 'kapasitas_sampah']]
y = df['waktu_sampai_penuh']

# Choose the linear regression model
model = RandomForestRegressor()

# Train the model
model.fit(X, y)