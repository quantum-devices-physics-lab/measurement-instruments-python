# Uso do QCodes
Um notebook com exemplo de código para controlar os equipamento do laboratório.

## Configuração da máquina local

#### clone o repositório
```bash
git clone https://github.com/QCoDeS/Qcodes.git
```

#### vá na pasta do repositório clonado e crie um environment conda
```bash
cd Qcodes
conda env create -f environment.yml
activate qcodes
```

#### Instale qcodes
```bash
pip install -e .
```

#### Crie um kernel IPython para o environment qcodes
```bash
python -m ipykernel install --user --name qcodes --display-name "QCodes Env"
```

Pronto! Basta iniciar o jupyter lab e escolher o kernel "QCodes Env"


#### Realtime plot

Instale jupyterplot com pip

```bash
pip install jupyterplot
```

