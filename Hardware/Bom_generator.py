import os
import pandas as pd

# Folder containing BOM CSV files

OUTPUT_FILE = "Combined_BOM.xlsx"

def combine_boms(input_folder, output_file):
    with pd.ExcelWriter(output_file, engine='openpyxl') as writer:

        for folder_name in os.listdir(input_folder):

            folder_path = os.path.join(input_folder, folder_name)

            # Skip if not a directory
            if not os.path.isdir(folder_path):
                continue

            for file in os.listdir(folder_path):

                if file.endswith(".csv"):

                    file_path = os.path.join(folder_path, file)

                    df = pd.read_csv(file_path)

                    # Sheet name = folder + file name
                    sheet_name = f"{folder_name}_{os.path.splitext(file)[0]}"
                    sheet_name = sheet_name[:31]  # Excel limit

                    df.to_excel(writer, sheet_name=sheet_name, index=False)

                    print(f"Added {file} from {folder_name}")

    print(f"\nCombined BOM saved as {output_file}")

if __name__ == "__main__":
    INPUT_FOLDER="Hardware"
    combine_boms(INPUT_FOLDER, OUTPUT_FILE)
